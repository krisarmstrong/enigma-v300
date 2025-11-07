# Enigma V300 - Python Functional Implementation

## Overview

This is a standalone Python implementation using a **functional programming** approach. All functionality is implemented using pure functions without classes.

- **Author**: Kris Armstrong
- **Version**: 3.0.1
- **License**: MIT

## Features

- Generates option keys for NetTool (10-digit serial) and other Fluke products (7-digit serial)
- Validates option keys against serial numbers and options
- Interactive menu with product and option selection
- Command-line support for automation
- Pure functional design - no classes, all functions are pure and composable

## Requirements

- Python 3.8 or later
- No external dependencies (uses only Python standard library)

## Usage

### Interactive Mode

Run the script without arguments to enter interactive mode:

```bash
python3 enigma_v300_functions.py
```

or make it executable:

```bash
chmod +x enigma_v300_functions.py
./enigma_v300_functions.py
```

### Command-Line Mode

Generate NetTool option key:
```bash
./enigma_v300_functions.py -n 0003333016 4
```

Validate NetTool option key:
```bash
./enigma_v300_functions.py -x 5dabade112dd 0003333016
```

Generate option key for other products:
```bash
./enigma_v300_functions.py -e 0000607 7 6963
```

Decrypt option key:
```bash
./enigma_v300_functions.py -d 6406257948597747
```

### Options

- `-n, --nettool SERIAL OPTION` - Generate NetTool key (10-digit serial, numeric option)
- `-x, --check-nettool KEY SERIAL` - Validate a NetTool key
- `-e, --encrypt SERIAL OPTION PRODUCT` - Create keys for other products (7-digit serial, 3-digit option, product code)
- `-d, --decrypt KEY` - Decode a key and print embedded serial/option/product information
- `--verbose` - Enable debug logging
- `--logfile FILE` - Log to file (rotates at 10MB)
- `--version` - Show version information

## Test Cases

**NetTool:**
```bash
./enigma_v300_functions.py -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

**EtherScope/MetroScope:**
```bash
./enigma_v300_functions.py -e 0000607 7 6963
# Output: Option Key: 6406 2579 4859 7747
```

## Implementation Details

This functional implementation provides:
- Pure functions with no side effects (except for I/O operations)
- Immutable data structures
- No global state (except constants)
- Composable, reusable functions
- Type hints for better code documentation

### Main Functions

- `enigma_c_encrypt(key)` / `enigma_c_decrypt(key)` - EnigmaC cipher for NetTool
- `enigma2_c_encrypt(key)` / `enigma2_c_decrypt(key)` - Enigma2C cipher for other products
- `generate_nettool_option_key(serial, option)` - Generate NetTool keys
- `check_nettool_option_key(option, key, serial)` - Validate NetTool keys
- `generate_enigma2_option_key(serial, option, product)` - Generate keys for other products
- `check_enigma2_option_key(option, key)` - Validate keys for other products
- `decode_enigma2_key(key)` - Decode and extract information from keys

## Comparison with Class-Based Version

While the class-based version (`extras/python_classes/`) organizes code using OOP principles, this functional version:
- Uses pure functions that are easier to test in isolation
- Has no mutable state
- Provides a more composable API
- May be preferred for functional programming enthusiasts

Choose the implementation that best fits your programming style and project requirements.

## License

This software is licensed under the MIT License. See the LICENSE file in the root directory for details.
