#!/usr/bin/env python3
"""

  # License
  <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 
  The licensor of this software is Silicon Laboratories Inc. Your use of this
  software is governed by the terms of Silicon Labs Master Software License
  Agreement (MSLA) available at
  www.silabs.com/about-us/legal/master-software-license-agreement. This
  software is distributed to you in Source Code format and is governed by the
  sections of the MSLA applicable to Source Code.


UART Console - Real-time log parser for sl_log_event_t records.

This console application receives ``sl_log_event_t`` binary records from a UART port
and decodes them into human-readable messages (record size is set by ``--max-args``).

Supports (sources are separate — core 0 never uses the descriptor; core 1 never uses .log_fmt):
- Core 0: pass `--out` (or auto-search) for `.log_fmt` from the .out/.axf/.elf; MESSAGE column only (EVENT is `-`).
- Core 1: pass `--descriptor` to a SystemView descriptor .txt. EVENT = debug_id; MESSAGE = rest of the row (no leading numeric id), printf args applied.

UART records match ``sl_log_event_t`` (``uint8_t`` ``arg_count``). Use ``--max-args N`` to match
``SL_LOG_CONFIG_ARG`` in firmware (fixed packet size ``8 + 4*N + 4`` bytes). Use ``--variable-packet`` if
the link sends the compact frame (length ``12 + 4*arg_count``).

Features:
- Auto-detection of JLink CDC UART port
- Auto-search for latest .out in C:\\Users\\surondla\\Logger\\ (recursive) by default
- Optional flashing (reset -> flash .out -> continue)
- Real-time log message decoding with timestamps
- DELTA column: difference between consecutive timestamps (per-core)
- Color-coded log levels (DEBUG, INFO, WARN, ERROR)
- Overflow detection and reporting
- Discard/resync logic to skip extra/misaligned bytes

Usage examples:
    python log_parser_core.py --out firmware.out --descriptor SYSVIEW_Si917nwp.txt
    python log_parser_core.py --max-args 6 --out firmware.out --descriptor SYSVIEW_Si917nwp.txt
    python log_parser_core.py --out firmware.out --descriptor SYSVIEW_Si917nwp.txt --port COM3
"""

import argparse
import struct
import sys
import time
import subprocess
import os
import re
from pathlib import Path
from typing import Dict, Optional, List, Tuple, Any, Sequence
from datetime import datetime

try:
    from elftools.elf.elffile import ELFFile
except ImportError:
    print("ERROR: pyelftools is required. Install with: pip install pyelftools")
    sys.exit(1)

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    print("ERROR: pyserial is required. Install with: pip install pyserial")
    sys.exit(1)

# Try to import colorama for colored output (optional)
try:
    from colorama import init, Fore, Style
    init(autoreset=True)
    COLORS_AVAILABLE = True
except ImportError:
    COLORS_AVAILABLE = False


# Upper bound for --max-args (argparse); must match firmware SL_LOG_CONFIG_ARG
SL_LOG_CONFIG_ARG_HARD_MAX = 32

# Core 1: debug_id in EVENT; MESSAGE = descriptor row without leading numeric event_id
EVENT_COLUMN_WIDTH = 48


def record_size_fixed(max_args: int) -> int:
    """Packed sl_log_event_t: II + max_args×I + BBBB (uint8 arg_count + core + flags + version)."""
    return 8 + 4 * max_args + 4

LEVEL_MAP = {1: "DEBUG", 2: "INFO", 3: "WARN", 4: "ERROR"}

LEVEL_COLORS = {
    "DEBUG": Fore.CYAN if COLORS_AVAILABLE else "",
    "INFO": Fore.GREEN if COLORS_AVAILABLE else "",
    "WARN": Fore.YELLOW if COLORS_AVAILABLE else "",
    "ERROR": Fore.RED if COLORS_AVAILABLE else "",
    "OVERFLOW": Fore.MAGENTA if COLORS_AVAILABLE else "",
}
RESET_COLOR = Style.RESET_ALL if COLORS_AVAILABLE else ""


# -------------------------------------------------------------------------
# Console UI helpers
# -------------------------------------------------------------------------
def print_banner():
    banner = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                         UART LOG CONSOLE                                     ║
║                    Real-time Log Message Decoder                             ║
╚══════════════════════════════════════════════════════════════════════════════╝
"""
    print(banner)


def print_config(
    port: str,
    baud: int,
    out_file: str,
    fmt_count: int,
    descriptor_path: Optional[str],
    descriptor_id_base: int = 0,
):
    print("┌─ Configuration ─────────────────────────────────────────────────────────────┐")
    print(f"│  Serial Port  : {port:<60} │")
    print(f"│  Baud Rate    : {baud:<60} │")
    print(f"│  OUT File     : {Path(out_file).name if out_file else 'None':<60} │")
    print(f"│  Format Strs  : {fmt_count:<60} │")
    print(f"│  Core1 descr. : {Path(descriptor_path).name if descriptor_path else 'None':<60} │")
    print(f"│  Descr. ID base: {descriptor_id_base:<59} │")
    print("└──────────────────────────────────────────────────────────────────────────────┘")
    print()
    print("Press Ctrl+C to stop the console.\n")
    sep_w = 132
    print("─" * sep_w)
    print(
        f"{'TIME':<12} {'LEVEL':<8} {'CORE':<6} {'TS':<12} {'DELTA':<12} "
        f"{'EVENT':<{EVENT_COLUMN_WIDTH}} MESSAGE"
    )
    print("─" * sep_w)


def get_current_time() -> str:
    now = datetime.now()
    return now.strftime("%H:%M:%S.") + f"{now.microsecond // 1000:03d}"


def colorize(text: str, level: str) -> str:
    if not COLORS_AVAILABLE:
        return text
    color = LEVEL_COLORS.get(level, "")
    return f"{color}{text}{RESET_COLOR}"


def format_event_column(event: Optional[str], width: int = EVENT_COLUMN_WIDTH) -> str:
    """EVENT column: core 1 symbolic name; '-' for core 0 / overflow."""
    if not event:
        return f"{'-':<{width}}"
    if len(event) > width:
        event = event[: width - 3] + "..."
    return f"{event:<{width}}"


# -------------------------------------------------------------------------
# Commander helpers
# -------------------------------------------------------------------------
def reset_device():
    try:
        print("Resetting device using commander...")
        subprocess.run(["commander", "device", "reset"], check=True)
        print("Device reset complete.")
    except Exception as e:
        print(f"ERROR: Failed to reset device: {e}")


def flash_out_file(out_path: str):
    """
    Flash using Simplicity Commander.
    Many setups accept: commander flash <file>
    If your environment requires extra args (device, serialno, etc.), adjust here.
    """
    try:
        print(f"Flashing firmware using commander: {out_path}")
        subprocess.run(["commander", "flash", out_path], check=True)
        print("Flash complete.")
    except Exception as e:
        print(f"ERROR: Failed to flash firmware: {e}")
        raise


# -------------------------------------------------------------------------
# Args
# -------------------------------------------------------------------------
def _parse_max_args(value: str) -> int:
    try:
        n = int(value, 10)
    except ValueError:
        raise argparse.ArgumentTypeError(f"invalid integer: {value!r}") from None
    if not 1 <= n <= SL_LOG_CONFIG_ARG_HARD_MAX:
        raise argparse.ArgumentTypeError(
            f"--max-args must be between 1 and {SL_LOG_CONFIG_ARG_HARD_MAX}, got {n}"
        )
    return n


def parse_args():
    p = argparse.ArgumentParser(
        description="UART log decoder: core 0 from .out (.log_fmt), core 1 from SystemView descriptor .txt.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples (core 0: .out | core 1: descriptor .txt):
  %(prog)s --out firmware.out --descriptor SYSVIEW_Si917nwp.txt
  %(prog)s --out firmware.out --descriptor SYSVIEW_Si917nwp.txt --flash
  %(prog)s --out firmware.out --descriptor SYSVIEW_Si917nwp.txt --port COM3 --baud 115200
        """,
    )

    p.add_argument("--port", help="Serial port name (override auto-detection).")
    p.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200).")

    p.add_argument(
        "--out", "--axf",
        dest="out_file",
        help=(
            "Path to .out/.axf/.elf for core 0 decode (.log_fmt section). If omitted, auto-search under --out-root."
        ),
    )

    p.add_argument(
        "--out-root",
        default=r"C:\Users\surondla\Logger",
        help="Root directory to recursively search for the latest .out/.axf/.elf if --out is omitted.",
    )

    p.add_argument(
        "--descriptor",
        "--manifest",
        dest="descriptor_path",
        metavar="PATH",
        help=(
            "SystemView descriptor .txt for core 1 (TAB-separated: event_id, debug_id, format). "
            "Example: SYSVIEW_Si917nwp.txt. Alias: --manifest."
        ),
    )

    p.add_argument(
        "--descriptor-id-base",
        type=int,
        default=0,
        metavar="N",
        help=(
            "Subtract N from each event_id in the descriptor file for UART lookup "
            "(0 when file ids match the wire; use 500 for SystemView exports that start at 500)."
        ),
    )

    p.add_argument(
        "--big-endian",
        action="store_true",
        help="Use big-endian for records (default: little-endian).",
    )

    p.add_argument(
        "--no-arg-format",
        action="store_true",
        help="Skip applying arguments to format strings (faster).",
    )

    p.add_argument(
        "--no-color",
        action="store_true",
        help="Disable colored output.",
    )

    p.add_argument(
        "--no-banner",
        action="store_true",
        help="Skip printing the console banner.",
    )

    p.add_argument(
        "--reset",
        action="store_true",
        help="Reset device using 'commander device reset' before starting.",
    )

    # Flashing option requested:
    p.add_argument(
        "--flash",
        action="store_true",
        help="Reset device, flash the selected .out file with commander, then continue.",
    )

    p.add_argument(
        "--raw",
        action="store_true",
        help="Also show raw hex data for each record (debug mode).",
    )

    p.add_argument(
        "--list-formats",
        action="store_true",
        help="List all format strings with their addresses from the .out file and exit.",
    )

    p.add_argument(
        "--max-args",
        type=_parse_max_args,
        default=10,
        metavar="N",
        help=(
            f"Number of uint32 slots in sl_log_event_t.args[] (must match SL_LOG_CONFIG_ARG in firmware). "
            f"Fixed packet size = 8 + 4*N + 4 bytes. Range: 1..{SL_LOG_CONFIG_ARG_HARD_MAX} (default: 10)."
        ),
    )

    p.add_argument(
        "--variable-packet",
        action="store_true",
        help=(
            "UART uses compact framing: timestamp, event_id, arg_count (u8), then arg_count uint32s, "
            "then core_id/flags/version (packet length = 12 + 4*arg_count). "
            "Otherwise the full packed struct is sent (length fixed from --max-args)."
        ),
    )

    return p.parse_args()


# -------------------------------------------------------------------------
# Record parsing
# -------------------------------------------------------------------------
def mk_struct_fmt(max_args: int, big_endian: bool) -> str:
    endian = ">" if big_endian else "<"
    # timestamp(4) + event_id(4) + args(max_args×4) + arg_count(1) + core(1) + flags(1) + version(1)
    return endian + ("II" + "I" * max_args + "BBBB")


def parse_record(chunk: bytes, fmt: str, max_args: int) -> Tuple[int, int, Tuple[int, ...], int, int, int, int]:
    ts, event_id, *rest = struct.unpack(fmt, chunk)
    args = tuple(rest[:max_args])
    arg_count, core_id, flags, version = rest[max_args : max_args + 4]
    return ts, event_id, args, arg_count, core_id, flags, version


def validate_log_fields(
    ts: int,
    event_id: int,
    arg_count: int,
    core_id: int,
    flags: int,
    version: int,
    max_args: int,
) -> bool:
    if arg_count > max_args:
        return False
    if ts == 0:
        return False
    if ts == 0xFFFFFFFF and event_id == 0xFFFFFFFF:
        return True
    lvl_code = (flags >> 1) & 0x07
    if lvl_code == 0 or lvl_code > 4:
        return False
    if version > 10:
        return False
    if core_id > 10:
        return False
    return True


def try_parse_variable_record(
    chunk: bytes, endian: str, max_args: int
) -> Optional[Tuple[int, int, Tuple[int, ...], int, int, int, int, int]]:
    """
    Compact UART framing: IIB + (arg_count × uint32) + BBB  →  length = 12 + 4*arg_count
    """
    if len(chunk) < 12:
        return None
    ts, eid, ac = struct.unpack(endian + "IIB", chunk[:9])
    if ac > max_args:
        return None
    need = 9 + 4 * ac + 3
    if len(chunk) < need:
        return None
    args_raw = list(struct.unpack(endian + ("I" * ac), chunk[9 : 9 + 4 * ac]))
    core_id, flags, version = struct.unpack("BBB", chunk[9 + 4 * ac : need])
    padded = tuple((args_raw + [0] * max_args)[:max_args])
    return ts, eid, padded, ac, core_id, flags, version, need


def format_raw_hex(chunk: bytes) -> str:
    return " ".join(f"{b:02X}" for b in chunk)


def level_from_flags(flags: int) -> str:
    lvl_code = (flags >> 1) & 0x07
    return LEVEL_MAP.get(lvl_code, f"LEVEL{lvl_code}")


# -------------------------------------------------------------------------
# OUT file format-string loading (core 0 typical path)
# -------------------------------------------------------------------------
def load_log_fmt_map(out_path: str) -> Dict[int, str]:
    try:
        with open(out_path, "rb") as f:
            elf = ELFFile(f)
            sec = elf.get_section_by_name(".log_fmt")
            if sec is None:
                print("Warning: .log_fmt section not found in OUT file.")
                print("         Core0 log messages may show as raw event IDs.")
                return {}

            base_addr = sec["sh_addr"]
            data = sec.data()
            n = len(data)

        fmt_map: Dict[int, str] = {}
        i = 0
        while i < n:
            if data[i] == 0:
                i += 1
                continue
            start = i
            while i < n and data[i] != 0:
                i += 1
            s = data[start:i].decode("utf-8", errors="replace")
            addr = base_addr + start
            fmt_map[addr] = s
            i += 1

        return fmt_map

    except Exception as e:
        print(f"ERROR: Failed to load format strings from {out_path}: {e}")
        return {}


def list_format_strings(fmt_map: Dict[int, str]):
    if not fmt_map:
        print("No format strings found in the .out file.")
        return

    print("\n" + "=" * 100)
    print("FORMAT STRINGS FROM .out FILE")
    print("=" * 100)
    print(f"Total format strings: {len(fmt_map)}\n")
    print(f"{'Address':<12} {'Hex Address':<14} Format String")
    print("-" * 100)

    for addr in sorted(fmt_map.keys()):
        fmt_str = fmt_map[addr]
        fmt_str_display = fmt_str.replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t")
        if len(fmt_str_display) > 70:
            fmt_str_display = fmt_str_display[:67] + "..."
        print(f"{addr:<12} 0x{addr:08X}    {fmt_str_display}")

    print("-" * 100)
    print(f"\nTotal: {len(fmt_map)} format strings\n")


# -------------------------------------------------------------------------
# SystemView descriptor (core 1 path)
# -------------------------------------------------------------------------
def _parse_systemview_descriptor_line(line: str) -> Optional[Tuple[int, str, str]]:
    """
    Parse one descriptor row. Supports:
    - TAB form: <event_id> TAB <debug_id>  <format>  (two spaces between debug_id and format)
    - Space form (e.g. SYSVIEW_CaptiveCore.txt): <event_id> <debug_id>  <format>
    Returns (event_id, debug_id, format) or None to skip the line.
    """
    stripped = line.strip()
    if not stripped or stripped.startswith("#"):
        return None
    if stripped.startswith("M=") or stripped.startswith("V="):
        return None

    if "\t" in line:
        left, right = line.split("\t", 1)
        try:
            eid = int(left.strip())
        except ValueError:
            return None
        right = right.strip()
        if "  " in right:
            debug_id, fmt = right.split("  ", 1)
            debug_id = debug_id.strip()
            fmt = fmt.strip()
        else:
            debug_id = right
            fmt = ""
        return eid, debug_id, fmt

    # Space-separated export: leading digits = event_id, then debug_id, then two spaces, then format
    if not stripped[0].isdigit():
        return None
    i = 0
    while i < len(stripped) and stripped[i].isdigit():
        i += 1
    if i == 0:
        return None
    try:
        eid = int(stripped[:i])
    except ValueError:
        return None
    rest = stripped[i:].lstrip()
    if "  " not in rest:
        return eid, rest.strip(), ""
    debug_id, fmt = rest.split("  ", 1)
    return eid, debug_id.strip(), fmt.strip()


def load_systemview_descriptor(
    descriptor_path: str, id_base: int = 0
) -> Dict[int, Dict[str, Any]]:
    """
    Parse a SystemView-style descriptor text file (e.g. SYSVIEW_Si917nwp.txt).

    Supported line shapes (after optional # comments; skip M=/V= headers):
    - TAB-separated: <event_id> TAB <debug_id>  <format>  (two spaces between debug_id and format)
    - Space-separated: <event_id> <debug_id>  <format>  (same two-space rule; e.g. SYSVIEW_CaptiveCore.txt)

    id_base: subtract from each file event_id before storing (e.g. 500 maps file row 500 → key 0).

    Returns mapping: event_id -> {debug_id, format, raw_line, component} (component always "").
    raw_line is the full non-comment row from the file; MESSAGE uses this with args applied.
    """
    out: Dict[int, Dict[str, Any]] = {}
    skipped_negative = 0
    try:
        with open(descriptor_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.rstrip("\n\r")
                parsed = _parse_systemview_descriptor_line(line)
                if not parsed:
                    continue
                eid, debug_id, fmt = parsed
                key = eid - id_base
                if key < 0:
                    skipped_negative += 1
                    continue
                out[key] = {
                    "debug_id": debug_id,
                    "format": fmt,
                    "raw_line": line,
                    "component": "",
                }
        print(
            f"Loaded {len(out)} entries from SystemView descriptor ({Path(descriptor_path).name})"
            f"{f' (id_base={id_base})' if id_base else ''}."
        )
        if skipped_negative:
            print(f"Note: skipped {skipped_negative} descriptor rows with id < id_base ({id_base}).")
        return out
    except Exception as e:
        print(f"ERROR: Failed to load SystemView descriptor {descriptor_path}: {e}")
        return {}


# -------------------------------------------------------------------------
# ELF string resolver (for %s pointers)
# -------------------------------------------------------------------------
class ELFStringResolver:
    """
    Best-effort: if firmware passes pointers for %s, try to read a null-terminated
    string from an allocatable PROGBITS section containing that address.
    """

    def __init__(self, out_path: str):
        self.out_path = out_path
        self._elf = None
        self._sections = []
        self._cache: Dict[int, str] = {}

        try:
            self._fp = open(out_path, "rb")
            self._elf = ELFFile(self._fp)
            for sec in self._elf.iter_sections():
                try:
                    sh_addr = sec["sh_addr"]
                    sh_size = sec["sh_size"]
                    sh_flags = sec["sh_flags"]
                    sh_type = sec["sh_type"]
                    # Keep allocatable, PROGBITS-like sections
                    if sh_addr and sh_size and (sh_flags & 0x2) and sh_type == "SHT_PROGBITS":
                        self._sections.append(sec)
                except Exception:
                    continue
        except Exception:
            self._elf = None

    def close(self):
        try:
            if getattr(self, "_fp", None):
                self._fp.close()
        except Exception:
            pass

    def read_cstring(self, addr: int, max_len: int = 256) -> Optional[str]:
        if addr in self._cache:
            return self._cache[addr]

        if not self._elf:
            return None

        for sec in self._sections:
            sh_addr = sec["sh_addr"]
            sh_size = sec["sh_size"]
            if sh_addr <= addr < (sh_addr + sh_size):
                offset = addr - sh_addr
                try:
                    data = sec.data()
                    end = min(len(data), offset + max_len)
                    raw = data[offset:end]
                    z = raw.find(b"\x00")
                    if z != -1:
                        raw = raw[:z]
                    s = raw.decode("utf-8", errors="replace")
                    # Heuristic: avoid returning junk
                    if s and any(ch.isprintable() for ch in s):
                        self._cache[addr] = s
                        return s
                except Exception:
                    continue
        return None


# -------------------------------------------------------------------------
# Argument formatting (improved printf-like handling)
# -------------------------------------------------------------------------
_PRINTF_SPEC_RE = re.compile(
    r"%(%)|%("                      # %% OR start of real spec
    r"(?:\d+\$)?"                   # positional (ignored)
    r"[#0\- +]*"                    # flags
    r"\d*"                          # width
    r"(?:\.\d+)?"                   # precision
    r"(?:hh|h|ll|l|z|t|j)?"         # length
    r"([diuoxXpsc]))"               # type
)

def _to_signed32(x: int) -> int:
    x &= 0xFFFFFFFF
    return x - 0x100000000 if x & 0x80000000 else x

def apply_args_to_format(fmt_str: str, args_list: List[int], arg_count: int,
                         resolver: Optional[ELFStringResolver] = None) -> str:
    """
    Best-effort C printf substitution supporting:
      %d %i %u %x %X %p %s %c plus width/precision/flags/length (mostly ignored)
    Also supports %%.
    """
    if not fmt_str:
        return fmt_str

    max_args = min(arg_count, len(args_list))
    arg_idx = 0

    def repl(m: re.Match) -> str:
        nonlocal arg_idx
        if m.group(1) == "%":   # %%
            return "%"

        spec_type = m.group(3)  # one of diuoxXpsc
        if arg_idx >= max_args:
            # Not enough args: leave the original token
            return m.group(0)

        val = args_list[arg_idx]
        arg_idx += 1

        if spec_type in ("d", "i"):
            return str(_to_signed32(val))
        if spec_type == "u":
            return str(val & 0xFFFFFFFF)
        if spec_type == "x":
            return f"{val & 0xFFFFFFFF:x}"
        if spec_type == "X":
            return f"{val & 0xFFFFFFFF:X}"
        if spec_type == "p":
            return f"0x{val & 0xFFFFFFFF:08x}"
        if spec_type == "c":
            return chr(val & 0xFF)
        if spec_type == "s":
            # Try pointer->string if we have an ELF resolver
            if resolver:
                s = resolver.read_cstring(val)
                if s is not None:
                    return s
            # Fallback: show pointer value
            return f"0x{val & 0xFFFFFFFF:08X}"

        return str(val)

    # Replace all specs sequentially
    return _PRINTF_SPEC_RE.sub(repl, fmt_str)


# -------------------------------------------------------------------------
# Serial helpers
# -------------------------------------------------------------------------
def find_jlink_port() -> Optional[str]:
    desired_ports = ["JLink CDC Uart Port", "JLink CDC UART", "J-Link"]
    for p in list_ports.comports():
        desc = p.description or ""
        for desired in desired_ports:
            if desired.lower() in desc.lower():
                return p.device
    return None


def list_available_ports():
    ports = list_ports.comports()
    if not ports:
        print("No serial ports found.")
        return
    print("\nAvailable serial ports:")
    for p in ports:
        print(f"  {p.device}: {p.description}")


def open_serial(port: str, baud: int) -> serial.Serial:
    ser = serial.Serial(
        port=port,
        baudrate=baud,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=0.0,
        write_timeout=0.0,
    )
    return ser


# -------------------------------------------------------------------------
# Latest .out finder (recursive)
# -------------------------------------------------------------------------
def find_latest_out_recursive(root_dir: Path) -> Optional[str]:
    if not root_dir.exists():
        return None
    out_files = []
    for ext in ("*.out", "*.axf", "*.elf"):
        out_files.extend(root_dir.rglob(ext))
    if not out_files:
        return None
    out_files.sort(key=lambda p: p.stat().st_mtime, reverse=True)
    return str(out_files[0])


def auto_find_out_file(out_root: str) -> Optional[str]:
    root = Path(out_root)
    chosen = find_latest_out_recursive(root)
    if chosen:
        print(f"Auto-detected latest OUT file under {root}: {chosen}")
        return chosen

    # fallback: old behavior (cwd common dirs)
    search_dirs = [
        Path.cwd() / "GNU ARM v12.2.1 - Default",
        Path.cwd() / "build",
        Path.cwd() / "Debug",
        Path.cwd() / "Release",
        Path.cwd(),
    ]
    out_files = []
    for base_dir in search_dirs:
        if base_dir.exists():
            out_files.extend(base_dir.rglob("*.out"))
            out_files.extend(base_dir.rglob("*.axf"))
            out_files.extend(base_dir.rglob("*.elf"))

    if not out_files:
        print("Warning: No .out/.axf/.elf files found.")
        return None

    out_files.sort(key=lambda p: p.stat().st_mtime, reverse=True)
    chosen = out_files[0]
    print(f"Auto-detected OUT file (fallback): {chosen}")
    return str(chosen)


# -------------------------------------------------------------------------
# Discard/resync logic (kept)
# -------------------------------------------------------------------------
def is_valid_record(chunk: bytes, fmt: str, max_args: int, record_size: int) -> bool:
    if len(chunk) != record_size:
        return False

    try:
        ts, event_id, *rest = struct.unpack(fmt, chunk)
        arg_count, core_id, flags, version = rest[max_args : max_args + 4]
        return validate_log_fields(ts, event_id, arg_count, core_id, flags, version, max_args)
    except Exception:
        return False


def find_record_sync(
    buffer: bytearray, fmt: str, max_args: int, record_size: int, max_search: int = 50
) -> int:
    if len(buffer) < record_size:
        return 0

    search_limit = min(max_search, len(buffer) - record_size + 1)
    for offset in range(search_limit):
        chunk = buffer[offset : offset + record_size]
        if is_valid_record(chunk, fmt, max_args, record_size):
            return offset

    return 1


def find_record_sync_variable(
    buffer: bytearray, endian: str, max_args: int, max_search: int = 120
) -> int:
    """Skip bytes to align to a valid compact record, or 0 if waiting for more data / aligned at 0."""
    if len(buffer) < 12:
        return 0

    # Do not consume bytes if a record at offset 0 is only incomplete (need more UART data)
    if len(buffer) >= 9:
        try:
            _ts, _eid, ac = struct.unpack(endian + "IIB", bytes(buffer[:9]))
            if ac <= max_args:
                need = 9 + 4 * ac + 3
                if len(buffer) < need:
                    return 0
        except Exception:
            pass

    search_limit = min(max_search, max(0, len(buffer) - 12))
    for offset in range(search_limit + 1):
        chunk = bytes(buffer[offset:])
        p = try_parse_variable_record(chunk, endian, max_args)
        if p is None:
            continue
        ts, eid, _, ac, core_id, flags, version, reclen = p
        if len(buffer) < offset + reclen:
            return 0
        if validate_log_fields(ts, eid, ac, core_id, flags, version, max_args):
            return offset

    # Truncated record starting at offset 0?
    if len(buffer) >= 9:
        try:
            _ts, _eid, ac = struct.unpack(endian + "IIB", bytes(buffer[:9]))
            if ac <= max_args and len(buffer) < 9 + 4 * ac + 3:
                return 0
        except Exception:
            pass

    return 1


# -------------------------------------------------------------------------
# Decode logic: core 0 only from .out .log_fmt; core 1 only from descriptor .txt
# Core 1: returns symbolic event name (debug_id) for EVENT column; MESSAGE = full descriptor row (args applied).
# -------------------------------------------------------------------------
def decode_message(event_id: int,
                   args_val: Sequence[int],
                   arg_count: int,
                   flags: int,
                   fmt_map: Dict[int, str],
                   manifest_map: Dict[int, Dict[str, Any]],
                   no_arg_format: bool,
                   core_id: int,
                   version: int,
                   resolver: Optional[ELFStringResolver],
                   max_args_supported: int = 10) -> Tuple[str, str, Optional[str]]:
    """
    Returns (level_str, message, event_display).
    event_display is the symbolic event name for core 1 (descriptor debug_id); None for core 0 / overflow.
    For core 1, message is the descriptor line after the event_id column (no numeric id), printf args applied.
    """
    # Overflow
    if event_id == 0xFFFFFFFF:
        overflow_count = args_val[0] if arg_count > 0 else 0
        return "OVERFLOW", f"Buffer overflow detected! Count: {overflow_count}", None

    level_str = level_from_flags(flags)
    arg_count = min(arg_count, max_args_supported)

    # Core 1 (e.g. NWP): decode only from SystemView descriptor — never from .out fmt_map
    if core_id == 1:
        if not manifest_map:
            return (
                level_str,
                "<Core1: no descriptor> Pass --descriptor <file.txt> (e.g. SYSVIEW_Si917nwp.txt); "
                "core 0 uses --out. "
                f"Args={list(args_val)[:arg_count]}",
                None,
            )
        entry = manifest_map.get(event_id)
        if entry:
            fmt_str = entry.get("format", "") or ""
            debug_id = (entry.get("debug_id", "") or "").strip()
            # EVENT column: symbolic name from descriptor (not numeric event_id)
            event_display = debug_id if debug_id else "unknown"

            # MESSAGE: descriptor row without leading numeric event_id; substitute % args on that text
            full_row = (entry.get("raw_line") or "").strip()
            if not full_row:
                full_row = (
                    f"{event_id}\t{debug_id}  {fmt_str.strip()}" if debug_id else str(event_id)
                )
            tab_prefix = f"{event_id}\t"
            if full_row.startswith(tab_prefix):
                body = full_row[len(tab_prefix) :]
            else:
                body = (
                    f"{debug_id}  {fmt_str.strip()}".strip()
                    if debug_id
                    else fmt_str.strip()
                )
            if not body:
                body = "(empty)"
            if (not no_arg_format) and arg_count > 0:
                msg = apply_args_to_format(body, list(args_val), arg_count, resolver=resolver)
            else:
                msg = body

            return level_str, msg, event_display

        # Unknown id: EVENT = unknown (no numeric id in columns; use --raw for the record)
        return (
            level_str,
            f"Not in descriptor. Args={list(args_val)[:arg_count]}",
            "unknown",
        )

    # Core 0 (and any core_id != 1): decode only from .out .log_fmt — never from descriptor
    is_fmt = (flags & 1) == 0
    if is_fmt and fmt_map:
        base_msg = fmt_map.get(event_id)
        if base_msg:
            if not no_arg_format and arg_count > 0:
                msg = apply_args_to_format(base_msg, list(args_val), arg_count, resolver=resolver)
            else:
                msg = base_msg
        else:
            msg = f"<Unknown Event 0x{event_id:08X}> Args={list(args_val)[:arg_count]}"
    else:
        msg = f"Event=0x{event_id:08X} Core={core_id} Ver={version} Args={list(args_val)[:arg_count]}"

    return level_str, msg, None


# -------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------
def main():
    global COLORS_AVAILABLE

    args = parse_args()

    if args.no_color:
        COLORS_AVAILABLE = False

    if not args.no_banner:
        print_banner()

    # Determine OUT file (needed for core0 format map, and also for %s pointer resolution)
    out_path = None
    if args.out_file:
        out_path = args.out_file
        if not os.path.exists(out_path):
            print(f"ERROR: OUT file not found: {out_path}")
            sys.exit(1)
    else:
        out_path = auto_find_out_file(args.out_root)
        if not out_path:
            print("ERROR: Could not find .out file automatically.")
            print("       Use --out <path> or adjust --out-root.")
            sys.exit(1)

    # Optional reset/flash logic
    # If --flash: do reset -> flash out -> continue.
    # If only --reset: just reset.
    if args.flash:
        reset_device()
        time.sleep(0.5)
        flash_out_file(out_path)
        time.sleep(0.5)
    elif args.reset:
        reset_device()
        time.sleep(0.5)

    # Load core0 format map
    fmt_map = load_log_fmt_map(out_path) if out_path else {}
    print(f"Loaded {len(fmt_map)} format strings from .log_fmt section")

    if args.list_formats:
        list_format_strings(fmt_map)
        sys.exit(0)

    # SystemView descriptor: required for decoding core 1 text (core 0 never reads this file)
    manifest_map: Dict[int, Dict[str, Any]] = {}
    if args.descriptor_path:
        if not os.path.exists(args.descriptor_path):
            print(f"ERROR: descriptor file not found: {args.descriptor_path}")
            sys.exit(1)
        manifest_map = load_systemview_descriptor(
            args.descriptor_path, id_base=args.descriptor_id_base
        )
    else:
        print(
            "Note: No --descriptor; core 1 lines stay as placeholders until you pass the descriptor .txt."
        )

    # String resolver (for %s pointers)
    resolver = ELFStringResolver(out_path) if out_path else None

    # Serial port selection
    if args.port:
        port_name = args.port
    else:
        port_name = find_jlink_port()
        if not port_name:
            print("ERROR: JLink CDC UART port not found.")
            list_available_ports()
            print("\nUse --port <port> to specify the serial port manually.")
            sys.exit(1)
        print(f"Auto-detected serial port: {port_name}")

    max_args = args.max_args
    endian_chr = ">" if args.big_endian else "<"
    record_size = record_size_fixed(max_args)
    rec_fmt = mk_struct_fmt(max_args, args.big_endian)

    if not args.variable_packet:
        sz = struct.calcsize(rec_fmt)
        if sz != record_size:
            print(f"ERROR: Struct size mismatch (got {sz}, expected {record_size}).")
            sys.exit(1)
        print(
            f"UART log record: fixed sl_log_event_t, max_args={max_args}, "
            f"{record_size} bytes/event (uint8 arg_count)."
        )
    else:
        print(
            f"UART log record: variable compact frame, max_args={max_args}, "
            f"length = 12 + 4*arg_count bytes."
        )

    try:
        ser = open_serial(port_name, args.baud)
    except Exception as e:
        print(f"ERROR: Failed to open serial port {port_name}: {e}")
        list_available_ports()
        sys.exit(1)

    print()
    print_config(
        port_name,
        args.baud,
        out_path or "",
        len(fmt_map),
        args.descriptor_path,
        descriptor_id_base=args.descriptor_id_base,
    )

    buffer = bytearray()
    read_size = 4096
    record_count = 0
    error_count = 0

    # Per-core last timestamp for DELTA column
    last_ts_by_core: Dict[int, Optional[int]] = {}

    def handle_one_record(
        ts: int,
        event_id: int,
        args_val: Tuple[int, ...],
        arg_count: int,
        core_id: int,
        flags: int,
        version: int,
        raw_chunk: bytes,
    ) -> None:
        nonlocal record_count, last_ts_by_core
        record_count += 1
        last_ts = last_ts_by_core.get(core_id)
        delta = (ts - last_ts) if (last_ts is not None) else None
        last_ts_by_core[core_id] = ts

        level_str, msg, event_disp = decode_message(
            event_id=event_id,
            args_val=args_val,
            arg_count=arg_count,
            flags=flags,
            fmt_map=fmt_map,
            manifest_map=manifest_map,
            no_arg_format=args.no_arg_format,
            core_id=core_id,
            version=version,
            resolver=resolver,
            max_args_supported=max_args,
        )
        event_col = format_event_column(event_disp)

        current_time = get_current_time()
        delta_str = f"{delta}" if delta is not None else "-"

        if args.raw:
            raw_hex = format_raw_hex(raw_chunk)
            output = (
                f"{current_time} [{colorize(level_str, level_str):>8}] "
                f"{core_id:<6} {ts:>10} {delta_str:>12} {event_col} {msg}\n"
                f"           RAW: {raw_hex}\n"
            )
        else:
            output = (
                f"{current_time} [{colorize(level_str, level_str):>8}] "
                f"{core_id:<6} {ts:>10} {delta_str:>12} {event_col} {msg}\n"
            )

        sys.stdout.write(output)
        sys.stdout.flush()

    try:
        while True:
            try:
                data = ser.read(read_size)
            except Exception as e:
                sys.stdout.write(f"\n[ERROR] Serial read failed: {e}\n")
                sys.stdout.flush()
                break

            if data:
                buffer.extend(data)

                if args.variable_packet:
                    while len(buffer) >= 12:
                        skip_bytes = find_record_sync_variable(buffer, endian_chr, max_args)
                        if skip_bytes > 0:
                            if skip_bytes >= len(buffer):
                                break
                            del buffer[:skip_bytes]
                            error_count += skip_bytes
                            continue

                        p = try_parse_variable_record(bytes(buffer), endian_chr, max_args)
                        if p is None:
                            # Incomplete record (need more bytes) — do not drop a byte
                            if len(buffer) >= 9:
                                try:
                                    _ts, _eid, ac = struct.unpack(
                                        endian_chr + "IIB", bytes(buffer[:9])
                                    )
                                    if ac <= max_args and len(buffer) < 9 + 4 * ac + 3:
                                        break
                                except Exception:
                                    pass
                            buffer.pop(0)
                            error_count += 1
                            continue

                        ts, event_id, args_val, arg_count, core_id, flags, version, reclen = p
                        if len(buffer) < reclen:
                            break
                        if not validate_log_fields(
                            ts, event_id, arg_count, core_id, flags, version, max_args
                        ):
                            buffer.pop(0)
                            error_count += 1
                            continue

                        raw_chunk = bytes(buffer[:reclen])
                        del buffer[:reclen]

                        try:
                            handle_one_record(
                                ts,
                                event_id,
                                args_val,
                                arg_count,
                                core_id,
                                flags,
                                version,
                                raw_chunk,
                            )
                        except Exception:
                            error_count += 1
                else:
                    while len(buffer) >= record_size:
                        skip_bytes = find_record_sync(buffer, rec_fmt, max_args, record_size)
                        if skip_bytes > 0:
                            if skip_bytes >= len(buffer):
                                break
                            del buffer[:skip_bytes]
                            error_count += skip_bytes
                            continue

                        if len(buffer) < record_size:
                            break

                        chunk = bytes(buffer[:record_size])

                        if not is_valid_record(chunk, rec_fmt, max_args, record_size):
                            buffer.pop(0)
                            error_count += 1
                            continue

                        del buffer[:record_size]

                        try:
                            ts, event_id, args_val, arg_count, core_id, flags, version = parse_record(
                                chunk, rec_fmt, max_args
                            )
                        except Exception:
                            error_count += 1
                            continue

                        handle_one_record(
                            ts,
                            event_id,
                            args_val,
                            arg_count,
                            core_id,
                            flags,
                            version,
                            chunk,
                        )

            else:
                time.sleep(0.001)

    except KeyboardInterrupt:
        print("\n" + "─" * 80)
        print(f"Console stopped. Records processed: {record_count}, Errors: {error_count}")
    finally:
        try:
            ser.close()
        except Exception:
            pass
        try:
            if resolver:
                resolver.close()
        except Exception:
            pass


if __name__ == "__main__":
    main()

