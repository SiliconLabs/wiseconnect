#!/usr/bin/env python3
"""
Simple HTTP server (GET/POST/PUT/HEAD/OPTIONS) with the same TCP teardown
semantics as case_http.py: raw socket I/O, response, wait for peer FIN,
then shutdown(SHUT_WR) and close.

PUT and Content-Length handling match case_http.py (no PUT_SUCCESS_STATUS /
PUT_RESPONSE_BODY). Verbose logs: SIMPLE_HTTP_VERBOSE=1

Python 3 recommended (matches case_http.py). Requires: future print for Py2 if used.
"""
from __future__ import print_function

import sys
import os
import json
import time
import socket
import mimetypes
import platform
from threading import Thread

READ_CHUNK = 4096
RECV_TIMEOUT = 30.0


def _env_verbose():
    return os.environ.get("SIMPLE_HTTP_VERBOSE", "").strip().lower() in (
        "1", "true", "yes", "on")


def log(msg):
    ts = time.strftime("%Y-%m-%d %H:%M:%S")
    print("[%s] %s" % (ts, msg))


def vlog(msg):
    if _env_verbose():
        log(msg)


def recv_until(sock, marker, max_bytes=1024 * 1024):
    data = b""
    while marker not in data:
        chunk = sock.recv(READ_CHUNK)
        if not chunk:
            break
        data += chunk
        if len(data) > max_bytes:
            break
    return data


def parse_headers(header_bytes):
    try:
        text = header_bytes.decode("iso-8859-1", errors="replace")
    except Exception:
        text = ""

    lines = text.split("\r\n")
    request_line = lines[0] if lines else ""
    headers = {}

    for line in lines[1:]:
        if not line or ":" not in line:
            continue
        k, v = line.split(":", 1)
        headers[k.strip().lower()] = v.strip()

    return request_line, headers


def build_response(method, path, body):
    raw_path = path.split("?", 1)[0].split("#", 1)[0]
    rel = os.path.normpath(raw_path.lstrip("/\\"))

    if method == "GET":
        if rel in ("", ".", os.path.sep):
            resp_body = b"WiseConnect HTTP server OK\n"
            ctype = "text/plain; charset=utf-8"
            status = "200 OK"
        else:
            full_path = os.path.join(".", rel)
            if os.path.isdir(full_path):
                resp_body = ("Directory access not allowed: %s" % raw_path).encode(
                    "utf-8")
                ctype = "text/plain; charset=utf-8"
                status = "404 Not Found"
            elif os.path.isfile(full_path):
                with open(full_path, "rb") as f:
                    resp_body = f.read()
                ctype, _ = mimetypes.guess_type(full_path)
                if not ctype:
                    ctype = "application/octet-stream"
                status = "200 OK"
            else:
                resp_body = ("File Not Found: %s" % raw_path).encode("utf-8")
                ctype = "text/plain; charset=utf-8"
                status = "404 Not Found"

    elif method == "PUT":
        if rel in ("", ".", ".."):
            resp_body = b'{"status":"error","message":"invalid path"}'
            ctype = "application/json; charset=utf-8"
            status = "400 Bad Request"
        else:
            dst_path = os.path.join(".", rel)
            parent = os.path.dirname(dst_path)
            if parent and not os.path.exists(parent):
                os.makedirs(parent, exist_ok=True)

            with open(dst_path, "wb") as f:
                f.write(body)

            payload = {
                "status": "ok",
                "saved_bytes": len(body),
                "path": rel,
            }
            resp_body = json.dumps(payload).encode("utf-8")
            ctype = "application/json; charset=utf-8"
            status = "200 OK"

    elif method == "POST":
        payload = {"status": "ok", "received_bytes": len(body)}
        resp_body = json.dumps(payload).encode("utf-8")
        ctype = "application/json; charset=utf-8"
        status = "200 OK"

    elif method == "HEAD":
        resp_body = b""
        ctype = "text/html; charset=utf-8"
        status = "200 OK"

    elif method == "OPTIONS":
        resp_body = b""
        ctype = "text/plain; charset=utf-8"
        status = "204 No Content"

    else:
        resp_body = b"Method Not Allowed\n"
        ctype = "text/plain; charset=utf-8"
        status = "405 Method Not Allowed"

    return status, ctype, resp_body


def compose_and_send(conn, status, ctype, resp_body, extra_headers):
    response = [
        ("HTTP/1.1 %s\r\n" % status).encode("ascii"),
        ("Content-Length: %d\r\n" % len(resp_body)).encode("ascii"),
        ("Content-Type: %s\r\n" % ctype).encode("ascii"),
        b"Connection: keep-alive\r\n",
    ]
    for h in extra_headers:
        response.append(h + b"\r\n")
    response.append(b"\r\n")
    conn.sendall(b"".join(response) + resp_body)


def wait_peer_fin_then_shut_wr(conn, client_ip, client_port):
    log("Waiting for DUT FIN from %s:%s ..." % (client_ip, client_port))
    while True:
        try:
            data = conn.recv(READ_CHUNK)
        except socket.timeout:
            log("Timeout waiting for DUT FIN from %s:%s" % (client_ip, client_port))
            break

        if data == b"":
            log("DUT FIN received from %s:%s" % (client_ip, client_port))
            log("Remote TCP stack has ACKed DUT FIN")
            break

        log("Received extra data after response: %d bytes" % len(data))

    try:
        conn.shutdown(socket.SHUT_WR)
        log("Remote shutdown(SHUT_WR) done, FIN should be sent now")
    except Exception as e:
        log("shutdown(SHUT_WR) failed: %s" % e)


def handle_client(conn, addr):
    client_ip, client_port = addr[0], addr[1]
    log("ACCEPT connection from %s:%s" % (client_ip, client_port))

    conn.settimeout(RECV_TIMEOUT)

    try:
        header_data = recv_until(conn, b"\r\n\r\n")
        if not header_data:
            log("No request data from %s:%s" % (client_ip, client_port))
            return

        if b"\r\n\r\n" in header_data:
            head, rest = header_data.split(b"\r\n\r\n", 1)
            head += b"\r\n\r\n"
        else:
            head = header_data
            rest = b""

        request_line, headers = parse_headers(head)
        vlog("Request-Line: %s" % request_line)

        try:
            method, path, version = request_line.split(" ", 2)
        except ValueError:
            method, path, version = "GET", "/", "HTTP/1.1"

        content_length = 0
        if "content-length" in headers:
            try:
                content_length = int(headers["content-length"])
            except Exception:
                content_length = 0

        if headers.get("expect", "").lower() == "100-continue":
            conn.sendall(b"HTTP/1.1 100 Continue\r\n\r\n")
            log("Sent 100 Continue")

        body = rest
        while len(body) < content_length:
            chunk = conn.recv(READ_CHUNK)
            if not chunk:
                break
            body += chunk

        if len(body) > content_length:
            body = body[:content_length]

        log(
            "Method=%s Path=%s Content-Length=%d Body-Read=%d"
            % (method, path, content_length, len(body))
        )

        status, ctype, resp_body = build_response(method, path, body)

        if method == "OPTIONS":
            extra_headers = [b"Allow: GET, POST, PUT, HEAD, OPTIONS"]
        else:
            extra_headers = []

        compose_and_send(conn, status, ctype, resp_body, extra_headers)
        log(
            "Response sent to %s:%s, bytes=%d"
            % (client_ip, client_port, len(resp_body))
        )

        wait_peer_fin_then_shut_wr(conn, client_ip, client_port)

    except Exception as e:
        log("Connection error with %s:%s -> %s" % (client_ip, client_port, e))

    finally:
        try:
            conn.close()
            log("CLOSE connection to %s:%s" % (client_ip, client_port))
        except Exception:
            pass


def run_server(port):
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("", port))
    srv.listen(5)

    host = "0.0.0.0"
    log("Server bind complete on %s:%d" % (host, port))
    log(
        "Listening on: http://%s:%d (Python %s, %s)"
        % (host, port, sys.version.split()[0], platform.system())
    )
    log("TCP teardown matches case_http.py (peer FIN, then SHUT_WR).")
    vlog("Set SIMPLE_HTTP_VERBOSE=1 for extra trace.")
    log("Press Ctrl+C to shut down the server and exit.")
    log("Awaiting new connection")

    try:
        while True:
            conn, addr = srv.accept()
            t = Thread(target=handle_client, args=(conn, addr))
            t.daemon = True
            t.start()
    except KeyboardInterrupt:
        log("Keyboard interrupt received, stopping server on port %d" % port)
    finally:
        try:
            srv.close()
        except Exception:
            pass


def run_on(port):
    run_server(port)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: simple_http_server.py <port> [<port> ...]")
        sys.exit(1)
    ports = []
    for arg in sys.argv[1:]:
        try:
            ports.append(int(arg))
        except ValueError:
            print("Port must be an integer: %s" % arg)
            sys.exit(1)
    for port_number in ports:
        server = Thread(target=run_on, args=(port_number,))
        server.daemon = True
        server.start()
    log("Server threads started for ports: %s" % ports)
    log("Press Ctrl+C to quit.")
    try:
        while True:
            time.sleep(1.0)
    except KeyboardInterrupt:
        pass
