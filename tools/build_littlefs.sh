#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/common.sh"

DATA_DIR="${1:-$DATA_DIR}"

print_banner

BIN_FILE="$BUILD_DIR/littlefs.bin"
TEMP_BIN="$BUILD_DIR/littlefs.tmp.bin"

echo "Building LittleFS image..."
echo

MKLITTLEFS=$(find \
  "$HOME/Library/Arduino15/packages/esp32/tools/mklittlefs" \
  -name mklittlefs -type f | sort | tail -1)

if [ ! -x "$MKLITTLEFS" ]; then
  echo "ERROR: mklittlefs not found."
  exit 1
fi

if [ ! -d "$DATA_DIR" ]; then
  echo "ERROR: data folder not found."
  exit 1
fi

mkdir -p "$BUILD_DIR"

rm -f "$BIN_FILE" "$TEMP_BIN"

PAYLOAD_BYTES=$(du -sk "$DATA_DIR" | awk '{print $1 * 1024}')
FILESYSTEM_BYTES=$((LITTLEFS_SIZE))

echo "Payload size:    $PAYLOAD_BYTES bytes"
echo "Filesystem size: $FILESYSTEM_BYTES bytes"
echo

if [ "$PAYLOAD_BYTES" -ge "$FILESYSTEM_BYTES" ]; then
  echo "ERROR: data payload is too large for the LittleFS partition."
  exit 1
fi

if ! "$MKLITTLEFS" \
  -c "$DATA_DIR" \
  -s "$LITTLEFS_SIZE" \
  "$TEMP_BIN"; then
  echo
  echo "ERROR: LittleFS image build failed."
  rm -f "$TEMP_BIN"
  exit 1
fi

if [ ! -f "$TEMP_BIN" ]; then
  echo "ERROR: LittleFS image was not created."
  exit 1
fi

mv "$TEMP_BIN" "$BIN_FILE"

echo
echo "LittleFS image built successfully:"
echo "$BIN_FILE"
