#!/bin/zsh

#
# Shared functions for Internet Radio Project tools
#

set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FOLDER_NAME="$(basename "$PROJECT_DIR")"

TOOLS_DIR="$PROJECT_DIR/tools"
DATA_DIR="$PROJECT_DIR/data"
BUILD_DIR="$PROJECT_DIR/build"

MAIN_SKETCH="$PROJECT_FOLDER_NAME.ino"

CONFIG_FILE="$TOOLS_DIR/project.conf"

source "$CONFIG_FILE"

print_banner() {
    echo
    echo "======================================"
    echo "  $PROJECT_NAME"
    echo "======================================"
    echo
}

print_section() {
    echo
    echo "---- $1 ----"
    echo
}

check_command_report() {
    if command -v "$1" >/dev/null 2>&1; then
        echo "✓ $1"
    else
        echo "✗ $1 missing"
        exit 1
    fi
}

report_tool() {
    local tool="$1"
    local label="$tool"
    local version_command="$tool --version"

    case "$tool" in
        git)
            label="Git"
            version_command="git --version"
            ;;
        python3)
            label="Python"
            version_command="python3 --version"
            ;;
        arduino-cli)
            label="Arduino CLI"
            version_command="arduino-cli version"
            ;;
        esptool)
            label="esptool"
            version_command="esptool version"
            ;;
    esac

    if command -v "$tool" >/dev/null 2>&1; then
        local version_output
        version_output=$(eval "$version_command" 2>/dev/null | head -n 1)
        echo "✓ $label - $version_output"
    else
        echo "✗ $label missing"
        exit 1
    fi
}

check_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        echo
        echo "ERROR: '$1' is not installed."
        exit 1
    fi
}