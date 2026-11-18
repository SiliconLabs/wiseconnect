#!/usr/bin/env python3
# ******************************************************************************
# @file  firmware_update_two_images_tcp_server.py
# @brief TCP server that serves two RPS images sequentially for firmware
#        fallback combined-image OTA (M4 + NWP) over a single connection.
# ******************************************************************************
# # License
# <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
# ******************************************************************************
#
# The licensor of this software is Silicon Laboratories Inc. Your use of this
# software is governed by the terms of Silicon Labs Master Software License
# Agreement (MSLA) available at
# www.silabs.com/about-us/legal/master-software-license-agreement. This
# software is distributed to you in Source Code format and is governed by the
# sections of the MSLA applicable to Source Code.
#
# ******************************************************************************

import socket
import sys
import os
import select

RPS_HEADER = 0x01
RPS_DATA = 0x00
RPS_HEADER_SIZE = 64
RPS_DATA_BLOCK_SIZE = 1024

def process_request(client_socket, file_paths):
    ctr = 0
    first_data_packet = True
    current_index = 0
    f = None
    file_size = 0
    try:
        while True:
            try:
                # Open next image file if needed
                if f is None and current_index < len(file_paths):
                    current_path = file_paths[current_index]
                    print(f"Opening image {current_index + 1}: {current_path}")
                    f = open(current_path, 'rb')
                    file_size = os.path.getsize(current_path)
                    first_data_packet = True

                print("waiting for recv")
                ready, _, _ = select.select([client_socket], [], [], 1.0)
                if client_socket in ready:
                    data = client_socket.recv(100)
                    print(f"recv length == 0x{len(data):x}")
                    if len(data) == 0:
                        print("client closed connection")
                        break

                    if data.startswith(b'EXIT'):
                        print("Exit command received. Shutting down.")
                        client_socket.close()
                        server_socket.close()
                        sys.exit(0)

                    cmd_type = data[0]
                    print(f"RX img {current_index + 1}: cmd=0x{cmd_type:02x} req_len={len(data)}")

                    # If no more images, just acknowledge end-of-data
                    if f is None and current_index >= len(file_paths):
                        client_socket.send(bytearray([RPS_DATA, 0, 0]))
                        continue

                    if cmd_type == RPS_HEADER:
                        # Always serve header from start of current image
                        f.seek(0)
                        chunk = f.read(RPS_HEADER_SIZE)
                        print(f"Header bytes read: {len(chunk)}; first8={chunk[:16].hex() if len(chunk) >= 16 else chunk.hex()}")
                    elif cmd_type == RPS_DATA:
                        # Start data stream from beginning on first data packet for each image
                        if first_data_packet:
                            f.seek(0)
                            first_data_packet = False
                        chunk = f.read(RPS_DATA_BLOCK_SIZE)
                    else:
                        continue

                    data1 = bytearray([cmd_type, len(chunk) & 0xff, (len(chunk) >> 8) & 0xff]) + chunk
                    print(f"TX img {current_index + 1}: prefix=[{data1[0]:02x} {data1[1]:02x} {data1[2]:02x}] len={len(chunk)}")
                    client_socket.sendall(data1)
                    print(f"size of data1=={len(chunk)}")
                    print(f"send returns {len(data1)}")
                    print(f"Pkt sent no:{ctr + 1}")
                    ctr += 1

                    # Advance after the last data block, including when file size is an exact
                    # multiple of 1024 (short-read check alone would miss that case).
                    reached_eof = (cmd_type == RPS_DATA) and (f.tell() >= file_size)
                    if reached_eof:
                        print("reach end of file for current image; advancing to next if available")
                        # Only send EoI marker after the final image to avoid leaving leftover bytes before next header
                        if (current_index + 1) >= len(file_paths):
                            print(f"TX img {current_index + 1}: prefix=[{RPS_DATA:02x} 00 00] len=0 (EoI)")
                            client_socket.sendall(bytearray([RPS_DATA, 0, 0]))
                        if f is not None:
                            f.close()
                            f = None
                        current_index += 1
                        first_data_packet = True
                        # Loop continues to serve next image over same connection
            except KeyboardInterrupt:
                print("KeyboardInterrupt during recv. Shutting down.")
                client_socket.close()
                raise
    finally:
        if f is not None:
            f.close()
        client_socket.close()

if __name__ == "__main__":
    server_socket = None
    try:
        if len(sys.argv) < 4:
            print("Usage: python firmware_update_two_images_tcp_server.py <local port> <RPS file path 1> <RPS file path 2>")
            sys.exit(0)

        local_port = int(sys.argv[1])
        file_paths = [sys.argv[2], sys.argv[3]]

        missing = [p for p in file_paths if not os.path.exists(p)]
        if missing:
            print("Unable to open RPS file(s): " + ", ".join(missing))
            sys.exit(0)

        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind(('0.0.0.0', local_port))
        server_socket.listen(1)
        print("Listen passed")

        while True:
            try:
                print("waiting for client to connect")
                ready, _, _ = select.select([server_socket], [], [], 1.0)
                if server_socket in ready:
                    client_socket, addr = server_socket.accept()
                    print("accept success")
                    process_request(client_socket, file_paths)
            except KeyboardInterrupt:
                print("KeyboardInterrupt during accept. Shutting down.")
                break
    finally:
        if server_socket is not None:
            server_socket.close()
        sys.exit(0)
