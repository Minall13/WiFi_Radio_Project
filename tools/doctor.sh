#!/bin/zsh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/common.sh"

print_banner

print_section "Environment"

echo "Project : $PROJECT_NAME"
echo "Version : $VERSION"
echo "Board   : $BOARD_NAME"

print_section "Required Tools"

for tool in "${REQUIRED_TOOLS[@]}"; do
    report_tool "$tool"
done

print_section "Required Directories"

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$PROJECT_DIR/$dir" ]; then
        echo "✓ $dir/"
    else
        echo "✗ $dir/ missing"
        exit 1
    fi
done

print_section "Required Files"

if [ -f "$PROJECT_DIR/$MAIN_SKETCH" ]; then
    echo "✓ $MAIN_SKETCH"
else
    echo "✗ $MAIN_SKETCH missing"
    exit 1
fi

for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$PROJECT_DIR/$file" ]; then
        echo "✓ $file"
    else
        echo "✗ $file missing"
        exit 1
    fi
done

print_section "Summary"

echo "✓ Project environment ready"