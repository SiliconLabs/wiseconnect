#!/bin/sh
# SiWx91x bt_stack_bypass - attach external HCI UART on modern Linux (kernel 5.10+ / 6.x).
#
# Usage:
#   sudo ./hci_uart_attach.sh /dev/ttyUSB0 115200
#   sudo ./hci_uart_attach.sh -B /dev/ttyUSB0 -S 115200 -N
#   sudo ./hci_uart_attach.sh -d /dev/ttyUSB0 115200    # background (recommended on Pi)
#   sudo hciconfig hci0 up
#
# btattach must stay running while hci0 is in use. Ctrl+C in the foreground kills it
# and removes hci0 (same as legacy hciattach when stopped).

set -e

DEVICE=""
BAUD="115200"
DAEMON=0

usage() {
	echo "Usage: $0 [-d] [/dev/ttyUSBX] [115200]"
	echo "   or: $0 [-d] -B /dev/ttyUSBX -S 115200 [-N]"
	echo ""
	echo "  -d, --daemon   run btattach in background (survives terminal Ctrl+C)"
	echo "  Stop attach:    pkill -f 'btattach.*ttyUSBX'"
	exit "${1:-0}"
}

while [ $# -gt 0 ]; do
	case "$1" in
	-h|--help)
		usage 0
		;;
	-d|--daemon)
		DAEMON=1
		shift
		;;
	-B)
		shift
		DEVICE="${1:?missing argument for -B}"
		shift
		;;
	-B*)
		DEVICE="${1#-B}"
		shift
		;;
	-S)
		shift
		BAUD="${1:?missing argument for -S}"
		shift
		;;
	-S*)
		BAUD="${1#-S}"
		shift
		;;
	-N)
		shift
		;;
	-*)
		echo "Unknown option: $1" >&2
		usage 1
		;;
	*)
		if [ -z "$DEVICE" ]; then
			DEVICE="$1"
		else
			BAUD="$1"
		fi
		shift
		;;
	esac
done

DEVICE="${DEVICE:-/dev/ttyUSB0}"

if ! command -v btattach >/dev/null 2>&1; then
	echo "btattach not found. Install BlueZ user tools, e.g.: apt install bluez" >&2
	exit 1
fi

if [ ! -e "$DEVICE" ]; then
	echo "Serial device not found: $DEVICE" >&2
	exit 1
fi

if command -v systemctl >/dev/null 2>&1; then
	systemctl stop ModemManager 2>/dev/null || true
	systemctl stop bluetooth 2>/dev/null || true
fi

pkill -f "btattach.*${DEVICE}" 2>/dev/null || true
pkill hciattach 2>/dev/null || true

stty -F "$DEVICE" "$BAUD" raw -echo -ixon -ixoff cs8 -cstopb -parenb -parodd crtscts

echo "Attaching SiWx91x HCI UART on $DEVICE @ $BAUD (H4, no host setup)..."
echo "Ensure bt_stack_bypass is running and Wi-Fi init completed before 'hciconfig hciX up'."

if [ "$DAEMON" -eq 1 ]; then
	nohup btattach -B "$DEVICE" -S "$BAUD" -P h4 -N >/dev/null 2>&1 &
	BTATTACH_PID=$!
	sleep 1
	if kill -0 "$BTATTACH_PID" 2>/dev/null; then
		echo "btattach running in background (PID $BTATTACH_PID)."
		echo "Next: sudo hciconfig hciX up"
		echo "Stop: pkill -f 'btattach.*${DEVICE}'"
	else
		echo "btattach failed to start." >&2
		exit 1
	fi
else
	echo "Running in foreground. Ctrl+C stops btattach and removes hciX."
	echo "Use -d to run in background."
	exec btattach -B "$DEVICE" -S "$BAUD" -P h4 -N
fi
