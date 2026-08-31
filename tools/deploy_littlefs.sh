#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

"$SCRIPT_DIR/build_littlefs.sh"
echo
"$SCRIPT_DIR/upload_littlefs.sh"