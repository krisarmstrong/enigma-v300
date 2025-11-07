# Enigma V300 - Python Class-Based Implementation

## Overview

This is a standalone Python implementation using an **object-oriented programming** approach. All functionality is organized into classes with encapsulation and clear separation of concerns.

- **Author**: Kris Armstrong
- **Version**: 3.0.1
- **License**: MIT

## Features

- Generates option keys for NetTool (10-digit serial) and other Fluke products (7-digit serial)
- Validates option keys against serial numbers and options
- Interactive menu with product and option selection
- Command-line support for automation
- Object-oriented design with clear class hierarchies

## Requirements

- Python 3.8 or later
- No external dependencies (uses only Python standard library)

## Usage

### Interactive Mode

Run the script without arguments to enter interactive mode:

```bash
python3 enigma_v300_classes.py
```

or make it executable:

```bash
chmod +x enigma_v300_classes.py
./enigma_v300_classes.py
```

### Command-Line Mode

Generate NetTool option key:
```bash
./enigma_v300_classes.py -n 0003333016 4
```

Validate NetTool option key:
```bash
./enigma_v300_classes.py -x 5dabade112dd 0003333016
```

Generate option key for other products:
```bash
./enigma_v300_classes.py -e 0000607 7 6963
```

Decrypt option key:
```bash
./enigma_v300_classes.py -d 6406257948597747
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
./enigma_v300_classes.py -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

**EtherScope/MetroScope:**
```bash
./enigma_v300_classes.py -e 0000607 7 6963
# Output: Option Key: 6406 2579 4859 7747
```

## Implementation Details

This class-based implementation provides:
- `EnigmaC` class - Handles NetTool (10-digit serial) encryption and validation
- `Enigma2C` class - Handles other Fluke products (7-digit serial) encryption and validation
- `EnigmaMenu` class - Provides interactive menu and orchestration
- Encapsulation of cipher logic and rotor tables
- Clean separation of concerns

### Main Classes

**EnigmaC**
- `encrypt(input_key)` - Encrypt using EnigmaC cipher
- `decrypt(input_key)` - Decrypt using EnigmaC cipher
- `check_option_key(option, key, serial)` - Validate NetTool keys

**Enigma2C**
- `encrypt(input_key)` - Encrypt using Enigma2C cipher
- `decrypt(input_key)` - Decrypt using Enigma2C cipher
- `check_option_key(option, key)` - Validate keys for other products

**EnigmaMenu**
- `calculate_nettool_option_key(serial, option)` - Generate NetTool keys
- `check_nettool_option_key(key, serial)` - Validate NetTool keys
- `calculate_enigma2_option_key(serial, option, product)` - Generate keys for other products
- `check_enigma2_option_key(key)` - Decode and validate keys
- `main_menu()` - Interactive menu interface

## Comparison with Functional Version

While the functional version (`extras/python_functions/`) uses pure functions, this class-based version:
- Organizes related functionality into cohesive classes
- Encapsulates cipher state and constants within classes
- Provides a more traditional OOP interface
- May be preferred for developers familiar with OOP patterns

Choose the implementation that best fits your programming style and project requirements.

## License

This software is licensed under the MIT License. See the LICENSE file in the root directory for details.
