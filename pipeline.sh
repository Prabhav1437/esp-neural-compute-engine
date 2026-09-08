#!/usr/bin/env bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

if [ -f "$SCRIPT_DIR/.venv/bin/activate" ]; then
    source "$SCRIPT_DIR/.venv/bin/activate"
fi

ACTION="${1:-all}"

case "$ACTION" in
    build)
        echo "==> Building ESP32 Firmware..."
        pio run
        ;;
    upload)
        echo "==> Uploading Firmware to ESP32..."
        pio run -t upload
        ;;
    monitor)
        echo "==> Starting Interactive Console (Local Echo + Send-On-Enter)..."
        python3 terminal.py
        ;;
    pio-monitor)
        echo "==> Starting PlatformIO Device Monitor..."
        pio device monitor
        ;;
    all|*)
        echo "=================================================="
        echo "    ESP32 Interactive Build, Upload & Console    "
        echo "=================================================="
        echo "==> Step 1/2: Compiling & Uploading Firmware..."
        pio run -t upload
        echo ""
        echo "==> Step 2/2: Launching Interactive Console..."
        echo "    (Type input at 'ESP32-Input> ' prompt and press Enter)"
        echo "--------------------------------------------------"
        python3 terminal.py
        ;;
esac
