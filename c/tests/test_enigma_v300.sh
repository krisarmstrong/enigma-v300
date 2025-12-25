#!/usr/bin/env bash
# Comprehensive tests for Enigma V300 C implementation
# Verified against V200 reference implementation

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/../build"
ENIGMA="${BUILD_DIR}/enigma_v300_pure_c"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Test helper functions
pass() {
    echo -e "${GREEN}PASS${NC}: $1"
    PASSED=$((PASSED + 1))
}

fail() {
    echo -e "${RED}FAIL${NC}: $1"
    echo "  Expected: $2"
    echo "  Got:      $3"
    FAILED=$((FAILED + 1))
}

check_output() {
    local test_name="$1"
    local expected="$2"
    local actual="$3"

    if [[ "$actual" == *"$expected"* ]]; then
        pass "$test_name"
    else
        fail "$test_name" "$expected" "$actual"
    fi
}

# Ensure binary exists
if [[ ! -x "$ENIGMA" ]]; then
    echo "Error: Binary not found at $ENIGMA"
    echo "Please build with: cmake -B build -S . && cmake --build build"
    exit 1
fi

echo "=== Enigma V300 C Implementation Tests ==="
echo ""

# Test 1: Version information
echo "--- Version Tests ---"
VERSION_OUTPUT=$("$ENIGMA" --version 2>&1 || true)
check_output "Version contains 3.0.0" "3.0.0" "$VERSION_OUTPUT"

# Test 2: Help output
HELP_OUTPUT=$("$ENIGMA" --help 2>&1 || true)
check_output "Help shows encrypt option" "-e" "$HELP_OUTPUT"
check_output "Help shows decrypt option" "-d" "$HELP_OUTPUT"

echo ""
echo "--- Encryption Tests (vs V200 Reference) ---"

# V200 Reference Test Vectors
# Serial 1234567, Option 7, Product 6963 -> 9225 9407 1950 7747
OUTPUT=$("$ENIGMA" -e 1234567 7 6963 2>&1)
check_output "EtherScope opt 7 encryption" "9225 9407 1950 7747" "$OUTPUT"

# Serial 1234567, Option 2, Product 7001 -> 8944 9371 5097 1162
OUTPUT=$("$ENIGMA" -e 1234567 2 7001 2>&1)
check_output "LinkRunner Pro opt 2 encryption" "8944 9371 5097 1162" "$OUTPUT"

# Serial 1234567, Option 4, Product 3001 -> 8042 7459 0175 9933
OUTPUT=$("$ENIGMA" -e 1234567 4 3001 2>&1)
check_output "NetTool S2 opt 4 encryption" "8042 7459 0175 9933" "$OUTPUT"

# Serial 0000607, Option 7, Product 6963 -> 6406 2579 4859 7747
OUTPUT=$("$ENIGMA" -e 0000607 7 6963 2>&1)
check_output "EtherScope reference case" "6406 2579 4859 7747" "$OUTPUT"

echo ""
echo "--- Decryption Tests (vs V200 Reference) ---"

# Decrypt 9225940719507747 -> Product 6963, Serial 1234567, Option 007
OUTPUT=$("$ENIGMA" -d 9225940719507747 2>&1)
check_output "Decrypt shows product 6963" "6963" "$OUTPUT"
check_output "Decrypt shows serial 1234567" "1234567" "$OUTPUT"
check_output "Decrypt shows option 007" "007" "$OUTPUT"

# Decrypt 8944937150971162 -> Product 7001, Serial 1234567, Option 002
OUTPUT=$("$ENIGMA" -d 8944937150971162 2>&1)
check_output "Decrypt shows product 7001" "7001" "$OUTPUT"
check_output "Decrypt shows option 002" "002" "$OUTPUT"

echo ""
echo "--- Product List Tests ---"
PRODUCTS_OUTPUT=$("$ENIGMA" --list-products 2>&1 || true)
check_output "List shows NetTool Series II" "NetTool Series II" "$PRODUCTS_OUTPUT"
check_output "List shows EtherScope" "EtherScope" "$PRODUCTS_OUTPUT"
check_output "List shows LinkRunner" "LinkRunner" "$PRODUCTS_OUTPUT"

echo ""
echo "--- Options List Tests ---"
OPTIONS_OUTPUT=$("$ENIGMA" --list-options 6964 2>&1 || true)
check_output "OneTouch options include Wired" "Wired" "$OPTIONS_OUTPUT"
check_output "OneTouch options include Wi-Fi" "Wi-Fi" "$OPTIONS_OUTPUT"

echo ""
echo "--- Edge Case Tests ---"

# All zeros serial
OUTPUT=$("$ENIGMA" -e 0000000 7 6963 2>&1)
check_output "All zeros serial works" "Option Key:" "$OUTPUT"

# All nines serial
OUTPUT=$("$ENIGMA" -e 9999999 7 6963 2>&1)
check_output "All nines serial works" "Option Key:" "$OUTPUT"

# Option 000
OUTPUT=$("$ENIGMA" -e 1234567 0 6963 2>&1)
check_output "Option 000 works" "Option Key:" "$OUTPUT"

echo ""
echo "=== Test Summary ==="
echo -e "Passed: ${GREEN}${PASSED}${NC}"
echo -e "Failed: ${RED}${FAILED}${NC}"

if [[ $FAILED -gt 0 ]]; then
    exit 1
fi
exit 0
