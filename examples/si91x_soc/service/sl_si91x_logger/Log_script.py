#!/usr/bin/env python3
"""
Log Console - lightweight real-time decoder for sl_log_event_t records.

Reads ``sl_log_event_t`` binary records from one of two transports and prints
them as human-readable lines:

  --source uart   serial port (default)
  --source rtt    SEGGER J-Link RTT (via pylink-square)

Decoding sources (kept separate by core_id in each record):
  * core 0  -> format strings come from the ELF .log_fmt section (--out)
  * core 1  -> format strings come from a SystemView descriptor .txt (--descriptor)

Packet layout (size is set at firmware compile time by SL_LOG_CONFIG_ARG):
  fixed : 8 + 4*N + 4 bytes, where N = --max-args = SL_LOG_CONFIG_ARG.

Usage:
  # UART
  python Log_script.py --out firmware.out --descriptor SYSVIEW_Si917nwp.txt
  python Log_script.py --out firmware.out --port COM7 --baud 115200

  # RTT (pip install pylink-square)
  python Log_script.py --source rtt --out firmware.out
  python Log_script.py --source rtt --device SiWG917M111M --rtt-channel 0 --rtt-channel2 8 --out firmware.out
"""

import argparse
import os
import re
import struct
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional, Sequence, Tuple

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

try:
    import pylink  # type: ignore
    PYLINK_AVAILABLE = True
except ImportError:
    pylink = None  # type: ignore
    PYLINK_AVAILABLE = False

try:
    from colorama import init, Fore, Style
    init(autoreset=True)
    COLORS_AVAILABLE = True
except ImportError:
    COLORS_AVAILABLE = False


SL_LOG_CONFIG_ARG_HARD_MAX = 32
EVENT_COLUMN_WIDTH = 36

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
def print_banner() -> None:
    print(
        "\n+================================================================+\n"
        "|                         LOG CONSOLE                            |\n"
        "|              Real-time sl_log_event_t decoder                  |\n"
        "+================================================================+\n"
    )


def print_config(
    source: str,
    transport_desc: str,
    out_file: str,
    fmt_count: int,
    descriptor_path: Optional[str],
    descriptor_id_base: int,
) -> None:
    print("+- Configuration ------------------------------------------------+")
    print(f"|  Source       : {source.upper():<46} |")
    print(f"|  Transport    : {transport_desc:<46} |")
    print(f"|  OUT File     : {(Path(out_file).name if out_file else 'None'):<46} |")
    print(f"|  Format Strs  : {fmt_count:<46} |")
    print(f"|  Core1 descr. : {(Path(descriptor_path).name if descriptor_path else 'None'):<46} |")
    print(f"|  Descr. ID base: {descriptor_id_base:<45} |")
    print("+----------------------------------------------------------------+\n")
    print("Press Ctrl+C to stop.\n")
    sep_w = 12 + 1 + 7 + 1 + 6 + 1 + 10 + 1 + 12 + 1 + EVENT_COLUMN_WIDTH + 1 + 30
    print("-" * sep_w)
    print(
        f"{'TIME':<12} {'LEVEL':<7} {'CORE':<6} {'TS':>10} {'DELTA':>12} "
        f"{'EVENT':<{EVENT_COLUMN_WIDTH}} MESSAGE"
    )
    print("-" * sep_w)


def get_current_time() -> str:
    now = datetime.now()
    return now.strftime("%H:%M:%S.") + f"{now.microsecond // 1000:03d}"


def colorize(text: str, level: str) -> str:
    if not COLORS_AVAILABLE:
        return text
    return f"{LEVEL_COLORS.get(level, '')}{text}{RESET_COLOR}"


def format_event_column(event: Optional[str], width: int = EVENT_COLUMN_WIDTH) -> str:
    if not event:
        return "-"
    if len(event) > width:
        event = event[: width - 3] + "..."
    return f"{event:<{width}}"


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


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description="Lightweight UART/RTT log decoder for sl_log_event_t records.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    p.add_argument("--source", choices=("uart", "rtt"), default="uart",
                   help="Transport: 'uart' (default) or 'rtt' (SEGGER J-Link RTT).")

    # UART
    p.add_argument("--port", help="[uart] Serial port name (override auto-detection).")
    p.add_argument("--baud", type=int, default=115200, help="[uart] Baud rate (default: 115200).")

    # RTT
    p.add_argument("--device", default="SiWG917M111M",
                   help="[rtt] J-Link target device name (default: SiWG917M111M). "
                        "Must be a device known to your J-Link installation - run "
                        "JLinkExe -CommandFile NUL and search for the part if unsure. "
                        "Common Si91x choices: SiWG917M111M, SiWG917M110L, "
                        "SiWG917M100M. Generic 'Cortex-M4' is rejected by some J-Link "
                        "versions for Si91x and is not recommended.")
    p.add_argument("--jlink-serial", dest="jlink_serial", default=None,
                   help="[rtt] J-Link probe serial number (optional).")
    p.add_argument("--rtt-channel", dest="rtt_channel", type=int, default=0,
                   help="[rtt] Primary RTT up-buffer channel (default: 0).")
    p.add_argument("--rtt-channel2", dest="rtt_channel2", type=int, default=None, metavar="N",
                   help="[rtt] Optional second RTT up-buffer channel (e.g. 8 for TA/NWP).")
    p.add_argument("--rtt-interface", dest="rtt_interface", choices=("swd", "jtag"),
                   default="swd", help="[rtt] J-Link interface (default: swd).")
    p.add_argument("--rtt-speed", dest="rtt_speed", default="1000",
                   help="[rtt] J-Link speed in kHz, or 'auto' (default: 1000). "
                        "Si91x debug ports can be unstable at the higher speeds "
                        "that 'auto' negotiates, leading to 'Could not start CPU "
                        "core' on connect; 1000 kHz is a safe baseline.")
    p.add_argument("--rtt-block-address", dest="rtt_block_address",
                   type=lambda v: int(v, 0), default=None,
                   help="[rtt] Fixed RTT control block address (e.g. 0x20000000). "
                        "If omitted, taken from --out (symbol _SEGGER_RTT or magic scan), "
                        "else J-Link RAM auto-scan.")
    p.add_argument("--rtt-reset", dest="rtt_reset", action="store_true",
                   help="[rtt] Reset the target on connect. Default is to attach to the "
                        "running firmware without resetting (Si91x debug ports can refuse "
                        "the halt that reset performs, producing 'Could not start CPU core').")
    p.add_argument("--rtt-no-reset", dest="rtt_no_reset", action="store_true",
                   help="[rtt] Kept for backward compatibility. Has no effect - no-reset "
                        "is now the default. Pass --rtt-reset to opt in to a reset.")
    p.add_argument("--rtt-connect-timeout", dest="rtt_connect_timeout",
                   type=float, default=18.0, metavar="SECS",
                   help="[rtt] Seconds to wait for the RTT control block (default: 18.0). "
                        "Firmware needs time to boot and call SEGGER_RTT_Init() before "
                        "UP buffers appear; increase if your firmware takes longer.")

    # Decode inputs
    p.add_argument("--out", "--axf", dest="out_file",
                   help="Path to .out/.axf/.elf for core 0 .log_fmt decoding "
                        "(also used to locate _SEGGER_RTT and to resolve %%s pointers).")
    p.add_argument("--descriptor", "--manifest", dest="descriptor_path", metavar="PATH",
                   help="SystemView descriptor .txt for core 1 decoding.")
    p.add_argument("--descriptor-id-base", type=int, default=0, metavar="N",
                   help="Subtract N from each descriptor event_id (default: 0).")

    # Packet shape (fixed sl_log_event_t; firmware always emits this layout)
    p.add_argument("--max-args", type=_parse_max_args, default=10, metavar="N",
                   help=f"Number of uint32 args slots per record. MUST match "
                        f"SL_LOG_CONFIG_ARG in the firmware build, otherwise framing "
                        f"will not align. Record size on the wire = 8 + 4*N + 4 bytes. "
                        f"Range 1..{SL_LOG_CONFIG_ARG_HARD_MAX} (default 10).")
    p.add_argument("--big-endian", action="store_true",
                   help="Use big-endian (default: little-endian).")

    # Display / logging
    p.add_argument("--no-arg-format", action="store_true",
                   help="Skip applying %% args to format strings.")
    p.add_argument("--no-color", action="store_true", help="Disable colored output.")
    p.add_argument("--no-banner", action="store_true", help="Skip console banner.")
    p.add_argument("--log-file", metavar="PATH",
                   help="Also write decoded output to this file (plain text by default).")
    p.add_argument("--log-file-color", action="store_true",
                   help="Keep ANSI colour codes in --log-file output.")

    # Utility
    p.add_argument("--list-formats", action="store_true",
                   help="List all format strings (with addresses) from the --out file and exit.")

    return p.parse_args()


# -------------------------------------------------------------------------
# Record parsing
# -------------------------------------------------------------------------
def record_size_fixed(max_args: int) -> int:
    return 8 + 4 * max_args + 4


def mk_struct_fmt(max_args: int, big_endian: bool) -> str:
    endian = ">" if big_endian else "<"
    return endian + ("II" + "I" * max_args + "BBBB")


def parse_record(chunk: bytes, fmt: str, max_args: int) -> Tuple[int, int, Tuple[int, ...], int, int, int, int]:
    ts, event_id, *rest = struct.unpack(fmt, chunk)
    args = tuple(rest[:max_args])
    arg_count, core_id, flags, version = rest[max_args : max_args + 4]
    return ts, event_id, args, arg_count, core_id, flags, version


# Populated by main() once the .log_fmt section is loaded.  These bound the
# valid address range for core-0 event_ids (each core-0 event_id is the flash
# address of its format string in .log_fmt).  Used by validate_log_fields()
# to reject misaligned frames whose "event_id" landed on the trailer of the
# previous record.
_FMT_ADDR_MIN: int = 0
_FMT_ADDR_MAX: int = 0
_MANIFEST_MAX_ID: int = 0


def set_event_id_bounds(fmt_addr_min: int, fmt_addr_max: int,
                        manifest_max_id: int) -> None:
    """Configure the per-core event_id sanity-check ranges used by the validator."""
    global _FMT_ADDR_MIN, _FMT_ADDR_MAX, _MANIFEST_MAX_ID
    _FMT_ADDR_MIN = int(fmt_addr_min)
    _FMT_ADDR_MAX = int(fmt_addr_max)
    _MANIFEST_MAX_ID = int(manifest_max_id)


def validate_log_fields(ts: int, event_id: int, arg_count: int,
                        core_id: int, flags: int, version: int, max_args: int) -> bool:
    """Heuristics that reject obvious junk so resync can find a real record.

    Tightened to reduce false-positive matches when --max-args is small
    (e.g. 3 -> only 24-byte records) and the RTT buffer is being attached
    mid-stream so the first window of bytes is essentially random.
    """
    if arg_count > max_args:
        return False
    if ts == 0:
        return False
    if ts == 0xFFFFFFFF and event_id == 0xFFFFFFFF:
        return True
    lvl_code = (flags >> 1) & 0x07
    if lvl_code == 0 or lvl_code > 4:
        return False
    # Si91x firmware uses sl_log_event_t version 1; allow 1..2 for forward
    # compat but reject random byte values.
    if version == 0 or version > 2:
        return False
    # Only core 0 (M4) and core 1 (NWP/TA) exist on Si91x.
    if core_id > 1:
        return False
    # Reserved high bits of flags (bit 4 and up) should be zero in current
    # firmware - random bytes will frequently have them set.
    if flags & 0xF0:
        return False
    # Event-id range sanity check (kills most resync false-positives).
    #   * core 0: event_id is the flash address of the format string in
    #             the ELF .log_fmt section. Anything outside [min..max] is
    #             a misalignment match (the most common one: the BBBB
    #             trailer of the previous record reinterpreted as event_id,
    #             producing values like 0x01020003 with version=1, flags=2).
    #   * core 1: event_id is a small integer descriptor key (or one of a
    #             few special markers). If a descriptor is loaded we reject
    #             event_ids that are too large to belong to it.
    if core_id == 0:
        if _FMT_ADDR_MAX and not (_FMT_ADDR_MIN <= event_id <= _FMT_ADDR_MAX):
            return False
    else:
        if _MANIFEST_MAX_ID and event_id > _MANIFEST_MAX_ID + 16:
            return False
    return True


def level_from_flags(flags: int) -> str:
    lvl_code = (flags >> 1) & 0x07
    return LEVEL_MAP.get(lvl_code, f"LEVEL{lvl_code}")


def is_valid_record(chunk: bytes, fmt: str, max_args: int, record_size: int) -> bool:
    if len(chunk) != record_size:
        return False
    try:
        ts, event_id, *rest = struct.unpack(fmt, chunk)
        arg_count, core_id, flags, version = rest[max_args : max_args + 4]
        return validate_log_fields(ts, event_id, arg_count, core_id, flags, version, max_args)
    except Exception:
        return False


def find_record_sync(buffer: bytearray, fmt: str, max_args: int,
                     record_size: int, max_search: int = 50) -> int:
    if len(buffer) < record_size:
        return 0
    search_limit = min(max_search, len(buffer) - record_size + 1)
    for offset in range(search_limit):
        if is_valid_record(buffer[offset : offset + record_size], fmt, max_args, record_size):
            return offset
    return 1


# -------------------------------------------------------------------------
# ELF helpers: .log_fmt, _SEGGER_RTT, %s string resolver
# -------------------------------------------------------------------------
def load_log_fmt_map(out_path: str) -> Tuple[Dict[int, str], int, int]:
    """Returns (addr -> format string map, section_base, section_end_exclusive)."""
    try:
        with open(out_path, "rb") as f:
            elf = ELFFile(f)
            sec = elf.get_section_by_name(".log_fmt")
            if sec is None:
                print("Warning: .log_fmt section not found in OUT file.")
                return {}, 0, 0
            base_addr = int(sec["sh_addr"])
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
            fmt_map[base_addr + start] = s
            i += 1
        return fmt_map, base_addr, base_addr + n
    except Exception as e:
        print(f"ERROR: Failed to load format strings from {out_path}: {e}")
        return {}, 0, 0


def list_format_strings(fmt_map: Dict[int, str]) -> None:
    """Pretty-print the .log_fmt strings loaded from the OUT file."""
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
        display = fmt_str.replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t")
        if len(display) > 70:
            display = display[:67] + "..."
        print(f"{addr:<12} 0x{addr:08X}    {display}")

    print("-" * 100)
    print(f"\nTotal: {len(fmt_map)} format strings\n")


def find_rtt_block_address(out_path: str) -> Optional[int]:
    """Find SEGGER RTT control block in an ELF: _SEGGER_RTT symbol, then magic-string scan."""
    if not out_path or not os.path.exists(out_path):
        return None
    try:
        with open(out_path, "rb") as f:
            elf = ELFFile(f)
            for sec in elf.iter_sections():
                iter_symbols = getattr(sec, "iter_symbols", None)
                if not callable(iter_symbols):
                    continue
                try:
                    for sym in iter_symbols():
                        if sym.name == "_SEGGER_RTT":
                            value = int(sym["st_value"])
                            if value:
                                return value & 0xFFFFFFFF
                except Exception:
                    continue

            magic = b"SEGGER RTT\x00\x00\x00\x00\x00\x00"
            for sec in elf.iter_sections():
                try:
                    sh_addr = int(sec["sh_addr"])
                    sh_size = int(sec["sh_size"])
                    sh_flags = int(sec["sh_flags"])
                    sh_type = sec["sh_type"]
                except Exception:
                    continue
                if not sh_addr or not sh_size:
                    continue
                if not (sh_flags & 0x2) or sh_type != "SHT_PROGBITS":
                    continue
                try:
                    data = sec.data()
                except Exception:
                    continue
                idx = data.find(magic)
                if idx != -1:
                    return (sh_addr + idx) & 0xFFFFFFFF
    except Exception as e:
        print(f"Warning: Could not scan {out_path} for RTT control block: {e}")
    return None


class ELFStringResolver:
    """Read null-terminated strings from PROGBITS sections for %s pointer args."""

    def __init__(self, out_path: str):
        self._fp = None
        self._elf = None
        self._sections: list = []
        self._cache: Dict[int, str] = {}
        try:
            self._fp = open(out_path, "rb")
            self._elf = ELFFile(self._fp)
            for sec in self._elf.iter_sections():
                try:
                    if (sec["sh_addr"] and sec["sh_size"]
                            and (sec["sh_flags"] & 0x2)
                            and sec["sh_type"] == "SHT_PROGBITS"):
                        self._sections.append(sec)
                except Exception:
                    continue
        except Exception:
            self._elf = None

    def close(self) -> None:
        try:
            if self._fp:
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
                try:
                    data = sec.data()
                    offset = addr - sh_addr
                    raw = data[offset : min(len(data), offset + max_len)]
                    z = raw.find(b"\x00")
                    if z != -1:
                        raw = raw[:z]
                    s = raw.decode("utf-8", errors="replace")
                    if s and any(ch.isprintable() for ch in s):
                        self._cache[addr] = s
                        return s
                except Exception:
                    continue
        return None


# -------------------------------------------------------------------------
# SystemView descriptor (core 1)
# -------------------------------------------------------------------------
def _parse_systemview_descriptor_line(line: str) -> Optional[Tuple[int, str, str]]:
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
            return eid, debug_id.strip(), fmt.strip()
        return eid, right, ""

    if not stripped[0].isdigit():
        return None
    i = 0
    while i < len(stripped) and stripped[i].isdigit():
        i += 1
    try:
        eid = int(stripped[:i])
    except ValueError:
        return None
    rest = stripped[i:].lstrip()
    if "  " not in rest:
        return eid, rest.strip(), ""
    debug_id, fmt = rest.split("  ", 1)
    return eid, debug_id.strip(), fmt.strip()


def load_systemview_descriptor(descriptor_path: str, id_base: int = 0) -> Dict[int, Dict[str, Any]]:
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
                out[key] = {"debug_id": debug_id, "format": fmt, "raw_line": line}
        print(
            f"Loaded {len(out)} entries from SystemView descriptor "
            f"({Path(descriptor_path).name})"
            f"{f' (id_base={id_base})' if id_base else ''}."
        )
        if skipped_negative:
            print(f"Note: skipped {skipped_negative} rows with id < id_base ({id_base}).")
        return out
    except Exception as e:
        print(f"ERROR: Failed to load SystemView descriptor {descriptor_path}: {e}")
        return {}


# -------------------------------------------------------------------------
# printf-style arg substitution
# -------------------------------------------------------------------------
_PRINTF_SPEC_RE = re.compile(
    r"%(%)|%("
    r"(?:\d+\$)?"
    r"[#0\- +]*"
    r"\d*"
    r"(?:\.\d+)?"
    r"(?:hh|h|ll|l|z|t|j)?"
    r"([diuoxXpsc]))"
)


def _to_signed32(x: int) -> int:
    x &= 0xFFFFFFFF
    return x - 0x100000000 if x & 0x80000000 else x


def apply_args_to_format(fmt_str: str, args_list: List[int], arg_count: int,
                         resolver: Optional[ELFStringResolver] = None) -> str:
    if not fmt_str:
        return fmt_str
    max_args = min(arg_count, len(args_list))
    arg_idx = 0

    def repl(m: re.Match) -> str:
        nonlocal arg_idx
        if m.group(1) == "%":
            return "%"
        spec = m.group(3)
        if arg_idx >= max_args:
            return m.group(0)
        val = args_list[arg_idx]
        arg_idx += 1
        if spec in ("d", "i"):
            return str(_to_signed32(val))
        if spec == "u":
            return str(val & 0xFFFFFFFF)
        if spec == "x":
            return f"{val & 0xFFFFFFFF:x}"
        if spec == "X":
            return f"{val & 0xFFFFFFFF:X}"
        if spec == "p":
            return f"0x{val & 0xFFFFFFFF:08x}"
        if spec == "c":
            return chr(val & 0xFF)
        if spec == "s":
            if resolver:
                s = resolver.read_cstring(val)
                if s is not None:
                    return s
            return f"0x{val & 0xFFFFFFFF:08X}"
        return str(val)

    return _PRINTF_SPEC_RE.sub(repl, fmt_str)


# -------------------------------------------------------------------------
# Transports: serial + RTT
# -------------------------------------------------------------------------
def find_jlink_port() -> Optional[str]:
    desired = ("JLink CDC Uart Port", "JLink CDC UART", "J-Link")
    for p in list_ports.comports():
        desc = p.description or ""
        for d in desired:
            if d.lower() in desc.lower():
                return p.device
    return None


def list_available_ports() -> None:
    ports = list_ports.comports()
    if not ports:
        print("No serial ports found.")
        return
    print("\nAvailable serial ports:")
    for p in ports:
        print(f"  {p.device}: {p.description}")


def open_serial(port: str, baud: int) -> serial.Serial:
    return serial.Serial(
        port=port, baudrate=baud,
        bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE, timeout=0.0, write_timeout=0.0,
    )


class RttReader:
    """Minimal non-blocking RTT reader with read(n)/close() like serial.Serial."""

    def __init__(self, device: str, channel: int = 0,
                 jlink_serial: Optional[str] = None, interface: str = "swd",
                 speed: Any = "auto", block_address: Optional[int] = None,
                 reset: bool = True, connect_timeout: float = 15.0):
        if not PYLINK_AVAILABLE:
            raise RuntimeError("pylink-square is required for --source rtt. "
                               "Install with: pip install pylink-square")
        if not device:
            raise RuntimeError("--device is required for --source rtt.")

        self._channel = int(channel)
        self._extra_channels: List[int] = []
        self._jlink = pylink.JLink()

        try:
            if jlink_serial:
                try:
                    self._jlink.open(serial_no=int(jlink_serial))
                except ValueError:
                    self._jlink.open(serial_no=jlink_serial)
            else:
                self._jlink.open()
        except Exception as e:
            raise RuntimeError(f"J-Link open failed: {e}") from e

        iface = (pylink.enums.JLinkInterfaces.JTAG
                 if str(interface).lower() == "jtag"
                 else pylink.enums.JLinkInterfaces.SWD)
        self._jlink.set_tif(iface)

        try:
            self._jlink.connect(device, speed=speed, verbose=False)
        except Exception as e:
            self._jlink.close()
            hint = ""
            if "unsupported device" in str(e).lower():
                hint = (" Hint: pass an exact device name known to your J-Link "
                        "installation via --device. For Si91x try SiWG917M111M, "
                        "SiWG917M110L or SiWG917M100M; generic 'Cortex-M4' is "
                        "rejected by recent J-Link versions for Si91x.")
            raise RuntimeError(f"J-Link connect failed: {e}.{hint}") from e

        if reset:
            try:
                self._jlink.reset(halt=False)
            except Exception:
                pass

        try:
            self._jlink.rtt_stop()
        except Exception:
            pass
        try:
            self._jlink.rtt_start(block_address)
        except Exception as e:
            self._jlink.close()
            raise RuntimeError(f"RTT start failed: {e}") from e

        deadline = time.time() + connect_timeout
        num_up = 0
        while time.time() < deadline:
            try:
                num_up = self._jlink.rtt_get_num_up_buffers()
                self._jlink.rtt_get_num_down_buffers()
                if num_up > 0:
                    break
            except Exception:
                pass
            time.sleep(0.1)

        if num_up == 0:
            self._jlink.close()
            raise RuntimeError(
                "Timed out waiting for RTT control block. Increase --rtt-connect-timeout, "
                "verify SEGGER_RTT is initialized in firmware, or pass --rtt-block-address."
            )

        if self._channel < 0 or self._channel >= num_up:
            self._jlink.close()
            raise RuntimeError(
                f"RTT up-channel {self._channel} not available "
                f"(target exposes {num_up} up-buffer(s))."
            )

        print(f"RTT connected: device={device}, iface={interface.upper()}, "
              f"speed={speed}, up_buffers={num_up}, channel={self._channel}")

        # Drain stale ring-buffer contents so parsing starts on a record
        # boundary. Two-phase drain:
        #   1. Hard ceiling so we never block forever if firmware is logging
        #      continuously at line rate.
        #   2. Quiescence detector: stop as soon as the buffer stays empty
        #      for ~80 ms in a row - that's the cleanest possible start.
        drained = self._drain_channel(self._channel, max_secs=1.5, quiet_ms=80)
        if drained:
            print(f"  Discarded {drained} stale RTT bytes from ch{self._channel} "
                  f"to align on a record boundary.")

    def add_channel(self, channel: int, num_up: int) -> None:
        ch = int(channel)
        if ch < 0 or ch >= num_up:
            raise RuntimeError(
                f"RTT up-channel {ch} not available (target exposes {num_up} up-buffer(s))."
            )
        if ch == self._channel or ch in self._extra_channels:
            return
        self._extra_channels.append(ch)
        drained = self._drain_channel(ch, max_secs=1.5, quiet_ms=80)
        if drained:
            print(f"  Discarded {drained} stale RTT bytes from ch{ch} "
                  f"to align on a record boundary.")

    def _drain_channel(self, channel: int, max_secs: float = 1.5,
                       quiet_ms: int = 80) -> int:
        """Drain a channel until it stays empty for ``quiet_ms`` ms (or hits
        the ``max_secs`` ceiling).  Returns the total byte count discarded.
        """
        drained = 0
        deadline = time.time() + max_secs
        quiet_until = time.time() + (quiet_ms / 1000.0)
        while time.time() < deadline:
            try:
                chunk = self._jlink.rtt_read(channel, 4096)
            except Exception:
                break
            if chunk:
                drained += len(chunk)
                quiet_until = time.time() + (quiet_ms / 1000.0)
            else:
                if time.time() >= quiet_until:
                    break
                time.sleep(0.01)
        return drained

    @property
    def num_up_buffers(self) -> int:
        try:
            return self._jlink.rtt_get_num_up_buffers()
        except Exception:
            return 0

    @property
    def all_channels(self) -> List[int]:
        return [self._channel] + self._extra_channels

    def read(self, size: int) -> bytes:
        return self.read_channel(self._channel, size)

    def read_channel(self, channel: int, size: int) -> bytes:
        try:
            chunk = self._jlink.rtt_read(channel, size)
        except Exception as e:
            raise IOError(f"RTT read (ch{channel}) failed: {e}") from e
        return bytes(chunk) if chunk else b""

    def close(self) -> None:
        try:
            self._jlink.rtt_stop()
        except Exception:
            pass
        try:
            self._jlink.close()
        except Exception:
            pass


# -------------------------------------------------------------------------
# Decode
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
    if event_id == 0xFFFFFFFF:
        overflow_count = args_val[0] if arg_count > 0 else 0
        return "OVERFLOW", f"Buffer overflow detected! Count: {overflow_count}", None

    level_str = level_from_flags(flags)
    arg_count = min(arg_count, max_args_supported)

    if core_id == 1:
        if not manifest_map:
            return (level_str,
                    f"<Core1: no descriptor> Pass --descriptor <file.txt>. "
                    f"Args={list(args_val)[:arg_count]}",
                    None)
        entry = manifest_map.get(event_id)
        if entry:
            fmt_str = entry.get("format", "") or ""
            debug_id = (entry.get("debug_id", "") or "").strip()
            event_display = debug_id if debug_id else "unknown"
            full_row = (entry.get("raw_line") or "").strip()
            if not full_row:
                full_row = (f"{event_id}\t{debug_id}  {fmt_str.strip()}"
                            if debug_id else str(event_id))
            tab_prefix = f"{event_id}\t"
            if full_row.startswith(tab_prefix):
                body = full_row[len(tab_prefix):]
            else:
                body = (f"{debug_id}  {fmt_str.strip()}".strip()
                        if debug_id else fmt_str.strip())
            if not body:
                body = "(empty)"
            if (not no_arg_format) and arg_count > 0:
                msg = apply_args_to_format(body, list(args_val), arg_count, resolver=resolver)
            else:
                msg = body
            return level_str, msg, event_display
        return (level_str, f"Not in descriptor. Args={list(args_val)[:arg_count]}", "unknown")

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
        msg = (f"Event=0x{event_id:08X} Core={core_id} Ver={version} "
               f"Args={list(args_val)[:arg_count]}")
    return level_str, msg, None


# -------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------
def main() -> None:
    global COLORS_AVAILABLE
    args = parse_args()

    if args.no_color:
        COLORS_AVAILABLE = False
    if not args.no_banner:
        print_banner()

    # OUT file is optional but recommended (core 0 format strings, %s strings,
    # and _SEGGER_RTT address for RTT auto-attach).
    out_path: Optional[str] = None
    if args.out_file:
        if not os.path.exists(args.out_file):
            print(f"ERROR: OUT file not found: {args.out_file}")
            sys.exit(1)
        out_path = args.out_file

    fmt_map: Dict[int, str] = {}
    fmt_sec_base = 0
    fmt_sec_end = 0
    if out_path:
        fmt_map, fmt_sec_base, fmt_sec_end = load_log_fmt_map(out_path)
        print(f"Loaded {len(fmt_map)} format strings from .log_fmt section "
              f"[0x{fmt_sec_base:08X}..0x{fmt_sec_end:08X})")

    if args.list_formats:
        if not out_path:
            print("ERROR: --list-formats requires --out <path-to-elf>.")
            sys.exit(1)
        list_format_strings(fmt_map)
        sys.exit(0)

    manifest_map: Dict[int, Dict[str, Any]] = {}
    if args.descriptor_path:
        if not os.path.exists(args.descriptor_path):
            print(f"ERROR: descriptor file not found: {args.descriptor_path}")
            sys.exit(1)
        manifest_map = load_systemview_descriptor(
            args.descriptor_path, id_base=args.descriptor_id_base
        )
    else:
        print("Note: No --descriptor; core 1 lines will be placeholders.")

    # Tell the record validator the valid event_id ranges so misaligned
    # frames whose "event_id" lands outside .log_fmt (core 0) or far past
    # the descriptor's max key (core 1) get rejected during resync. Use the
    # actual section bounds from the ELF rather than just min/max of the
    # parsed strings, so any address inside .log_fmt is accepted.
    manifest_max_id = max(manifest_map.keys()) if manifest_map else 0
    set_event_id_bounds(fmt_sec_base, max(0, fmt_sec_end - 1), manifest_max_id)

    resolver = ELFStringResolver(out_path) if out_path else None

    max_args = args.max_args
    endian_chr = ">" if args.big_endian else "<"
    record_size = record_size_fixed(max_args)
    rec_fmt = mk_struct_fmt(max_args, args.big_endian)

    sz = struct.calcsize(rec_fmt)
    if sz != record_size:
        print(f"Struct size mismatch (got {sz}, expected {record_size}).")
        sys.exit(1)
    print(f"Framing [{args.source}]: sl_log_event_t, max_args={max_args}, "
          f"{record_size} bytes/event "
          f"(must match firmware SL_LOG_CONFIG_ARG).")

    # ---- Open transport ----
    ser: Any
    transport_desc: str
    if args.source == "rtt":
        if not PYLINK_AVAILABLE:
            print("ERROR: --source rtt requires 'pylink-square'. "
                  "Install with: pip install pylink-square")
            sys.exit(1)

        rtt_block_addr = args.rtt_block_address
        block_source = "user"
        if rtt_block_addr is None and out_path:
            found = find_rtt_block_address(out_path)
            if found is not None:
                rtt_block_addr = found
                block_source = f"ELF ({Path(out_path).name})"
                print(f"RTT control block from {block_source}: 0x{rtt_block_addr:08X}")
            else:
                block_source = "J-Link auto-scan"
        elif rtt_block_addr is None:
            block_source = "J-Link auto-scan"

        try:
            ser = RttReader(
                device=args.device,
                channel=args.rtt_channel,
                jlink_serial=args.jlink_serial,
                interface=args.rtt_interface,
                speed=args.rtt_speed,
                block_address=rtt_block_addr,
                reset=args.rtt_reset,
                connect_timeout=args.rtt_connect_timeout,
            )
        except Exception as e:
            print(f"ERROR: Failed to open RTT: {e}")
            sys.exit(1)

        if args.rtt_channel2 is not None:
            try:
                ser.add_channel(args.rtt_channel2, ser.num_up_buffers)
                print(f"RTT channel {args.rtt_channel2} also enabled.")
            except Exception as e:
                print(f"WARNING: Could not add RTT channel {args.rtt_channel2}: {e}")

        block_desc = (f"0x{rtt_block_addr:08X} [{block_source}]"
                      if rtt_block_addr is not None else f"auto [{block_source}]")
        ch_desc = (f"ch{args.rtt_channel}+ch{args.rtt_channel2}"
                   if args.rtt_channel2 is not None else f"ch{args.rtt_channel}")
        transport_desc = (f"RTT {ch_desc} via J-Link "
                          f"({args.rtt_interface.upper()}, {args.device}, CB {block_desc})")
    else:
        port_name = args.port or find_jlink_port()
        if not port_name:
            print("ERROR: JLink CDC UART port not found.")
            list_available_ports()
            print("\nUse --port <port> to specify the serial port manually.")
            sys.exit(1)
        if not args.port:
            print(f"Auto-detected serial port: {port_name}")
        try:
            ser = open_serial(port_name, args.baud)
        except Exception as e:
            print(f"ERROR: Failed to open serial port {port_name}: {e}")
            list_available_ports()
            sys.exit(1)
        transport_desc = f"UART {port_name} @ {args.baud}"

    print()
    print_config(args.source, transport_desc, out_path or "",
                 len(fmt_map), args.descriptor_path,
                 descriptor_id_base=args.descriptor_id_base)

    # ---- Decode loop ----
    read_size = 4096
    record_count = 0
    # "Initial" = bytes/episodes discarded BEFORE the first valid record was
    # decoded - these come from attaching mid-stream / stale ring-buffer
    # contents and are not lost log messages.
    # "Runtime" = bytes/episodes discarded AFTER decoding started - these
    # usually indicate firmware overran the RTT up-buffer (the host wasn't
    # draining fast enough), so log messages WERE lost.
    bytes_initial = 0
    bytes_runtime = 0
    resync_initial = 0
    resync_runtime = 0
    last_ts_by_core: Dict[int, Optional[int]] = {}

    log_file = None
    if args.log_file:
        try:
            log_file = open(args.log_file, "w", encoding="utf-8", buffering=1)
            print(f"Saving decoded log to: {args.log_file}  "
                  f"[{'with ANSI colours' if args.log_file_color else 'plain text'}]")
        except Exception as e:
            print(f"WARNING: Cannot open log file '{args.log_file}': {e}.")

    _ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")

    _is_rtt_multi = (args.source == "rtt" and args.rtt_channel2 is not None)
    if _is_rtt_multi:
        _channel_buffers: Dict[int, bytearray] = {
            ch: bytearray() for ch in ser.all_channels  # type: ignore[union-attr]
        }
    else:
        _channel_buffers = {0: bytearray()}

    # ---- Pre-alignment: silently consume bytes until the first valid record
    # boundary so the main parser starts cleanly. Whatever we discard here is
    # the inevitable mid-record tail left over after the drain ended on a
    # time-based quiet window rather than a frame boundary, and would
    # otherwise show up as the "Bytes discarded (alignment)" overhead in the
    # final summary. Not counted toward bytes_initial.
    def _read_one(ch: int) -> bytes:
        if args.source == "rtt":
            try:
                return ser.read_channel(ch, read_size)  # type: ignore[union-attr]
            except Exception:
                return b""
        try:
            return ser.read(read_size)
        except Exception:
            return b""

    def _pre_align_channel(ch: int, buf: bytearray) -> int:
        """Walk forward until a valid record sits at offset 0 of buf. Returns
        the number of bytes silently consumed during alignment."""
        consumed = 0
        deadline = time.time() + 2.0
        min_window = record_size * 2
        while time.time() < deadline:
            data = _read_one(ch)
            if data:
                buf.extend(data)
            if len(buf) < min_window:
                if not data:
                    time.sleep(0.005)
                continue
            off = find_record_sync(buf, rec_fmt, max_args, record_size,
                                   max_search=min(256, len(buf) - record_size))
            if off == 0:
                return consumed
            if off > 0:
                if off >= len(buf):
                    consumed += off
                    del buf[:off]
                    continue
                consumed += off
                del buf[:off]
                if is_valid_record(bytes(buf[:record_size]), rec_fmt,
                                   max_args, record_size):
                    return consumed
            if not data:
                time.sleep(0.005)
        return consumed

    # Only run pre-alignment for RTT - UART starts at byte 0 of the stream
    # so it never has a mid-record tail to skip.  Runs silently so the
    # decoded-log stream isn't preceded by noise lines while debugging.
    if args.source == "rtt":
        for _ch, _buf in _channel_buffers.items():
            actual_ch = _ch if _is_rtt_multi else args.rtt_channel
            _pre_align_channel(actual_ch, _buf)

    def handle_one_record(ts: int, event_id: int, args_val: Tuple[int, ...],
                          arg_count: int, core_id: int, flags: int,
                          version: int) -> None:
        nonlocal record_count
        record_count += 1
        last_ts = last_ts_by_core.get(core_id)
        delta = (ts - last_ts) if (last_ts is not None) else None
        last_ts_by_core[core_id] = ts

        level_str, msg, event_disp = decode_message(
            event_id=event_id, args_val=args_val, arg_count=arg_count, flags=flags,
            fmt_map=fmt_map, manifest_map=manifest_map,
            no_arg_format=args.no_arg_format,
            core_id=core_id, version=version, resolver=resolver,
            max_args_supported=max_args,
        )
        event_col = format_event_column(event_disp)
        delta_str = f"{delta}" if delta is not None else "-"
        level_field = f"[{colorize(f'{level_str:>5}', level_str)}]"
        output = (f"{get_current_time()} {level_field} "
                  f"{core_id:<6} {ts:>10} {delta_str:>12} {event_col} {msg}\n")
        sys.stdout.write(output)
        sys.stdout.flush()
        if log_file:
            log_file.write(output if args.log_file_color else _ANSI_RE.sub("", output))

    def _account_discard(n: int, is_resync_episode: bool = False) -> None:
        """Charge ``n`` discarded bytes to either the initial or runtime bucket.

        Silent: counters are reported only in the final summary, not inline,
        so the live decoded-log stream stays clean for debugging.
        """
        nonlocal bytes_initial, bytes_runtime, resync_initial, resync_runtime
        if record_count == 0:
            bytes_initial += n
            if is_resync_episode:
                resync_initial += 1
        else:
            bytes_runtime += n
            if is_resync_episode:
                resync_runtime += 1

    def _parse_buffer(buf: bytearray) -> None:
        while len(buf) >= record_size:
            skip = find_record_sync(buf, rec_fmt, max_args, record_size)
            if skip > 0:
                if skip >= len(buf):
                    break
                del buf[:skip]
                _account_discard(skip, is_resync_episode=True)
                continue
            if len(buf) < record_size:
                break
            chunk = bytes(buf[:record_size])
            if not is_valid_record(chunk, rec_fmt, max_args, record_size):
                buf.pop(0)
                _account_discard(1)
                continue
            del buf[:record_size]
            try:
                ts, event_id, args_val, arg_count, core_id, flags, version = parse_record(
                    chunk, rec_fmt, max_args
                )
            except Exception:
                _account_discard(record_size)
                continue
            handle_one_record(ts, event_id, args_val, arg_count,
                              core_id, flags, version)

    _dead_channels: set = set()

    try:
        while True:
            got_any = False
            if _is_rtt_multi:
                primary_ch = args.rtt_channel
                for ch, buf in list(_channel_buffers.items()):
                    if ch in _dead_channels:
                        continue
                    try:
                        data = ser.read_channel(ch, read_size)  # type: ignore[union-attr]
                    except Exception as e:
                        if ch == primary_ch:
                            sys.stdout.write(f"\n[ERROR] RTT read (ch{ch}) failed: {e}\n")
                            raise KeyboardInterrupt
                        sys.stdout.write(f"\n[WARNING] RTT ch{ch} read failed: {e}. "
                                         f"Skipping ch{ch}.\n")
                        _dead_channels.add(ch)
                        continue
                    if data:
                        buf.extend(data)
                        got_any = True
                    _parse_buffer(buf)
            else:
                try:
                    data = ser.read(read_size)
                except Exception as e:
                    sys.stdout.write(f"\n[ERROR] Read failed: {e}\n")
                    break
                buf = _channel_buffers[0]
                if data:
                    buf.extend(data)
                    got_any = True
                _parse_buffer(buf)

            if not got_any:
                time.sleep(0.001)

    except KeyboardInterrupt:
        print("\n" + "-" * 64)
        print()
        print(f"Console stopped. Records decoded: {record_count}")
        if log_file:
            print(f"Log saved to: {args.log_file}")
    finally:
        try:
            ser.close()
        except Exception:
            pass
        if resolver:
            try:
                resolver.close()
            except Exception:
                pass
        if log_file:
            try:
                log_file.close()
            except Exception:
                pass


if __name__ == "__main__":
    main()