#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/common.sh"

print_banner

BIN_FILE="$BUILD_DIR/littlefs.bin"

check_command esptool

if [ ! -f "$BIN_FILE" ]; then
  echo "ERROR: LittleFS image not found."
  echo "Run ./tools/build_littlefs.sh first."
  exit 1
fi

echo "Uploading LittleFS image..."
echo "$BIN_FILE"
echo

esptool --chip "$BOARD_CHIP" --baud "$UPLOAD_BAUD" write-flash "$LITTLEFS_OFFSET" "$BIN_FILE"

echo
echo "LittleFS upload complete."