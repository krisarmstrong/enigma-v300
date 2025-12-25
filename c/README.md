# Enigma v3.0.0 - C Implementation

## Overview

This is a C11 implementation of the Fluke option key calculator. It generates and validates option keys for Fluke
network testing devices using EnigmaC (NetTool, 10-digit serial) and Enigma2C (other products, 7-digit serial) ciphers,
using standard C for portability.

- **Author**: Kris Armstrong
- **Version**: 3.0.0
- **License**: MIT

## Features

- Generates option keys for NetTool (10-digit serial) and other Fluke products (7-digit serial).
- Validates option keys against serial numbers and options.
- Interactive menu with product and option selection.
- Command-line support for automation.

## Requirements

- C11 compatible compiler (e.g., gcc 4.8+, clang 3.3+, MSVC 2015+)
- Standard C library

## Compilation

```bash
cmake -S . -B build
cmake --build build
```

Or compile directly:
```bash
gcc -std=c11 src/main.c src/enigma_v300_pure_c.c -o enigma
```

## Usage

Interactive mode:

```bash
./enigma
```

Command-line examples:

```bash
./enigma -n 0003333016 4  # NetTool key
./enigma -e 0000607 7 6963  # EtherScope key
./enigma --list-products  # Show known product codes
```

For quick reference:

- `-h`, `--help`, or `-?` prints usage details.
- `-V` or `--version` prints the application version.
- `--list-products` lists supported product codes.
- `--list-options <code>` lists available options for the specified product code.

## Test Cases

**NetTool:**

```bash
./enigma -n 0003333016 4
```

Output: Option Key: 5dab ade1 12dd

**EtherScope:**

```bash
./enigma -e 0000607 7 6963
```

Output: Option Key: 6406 2579 4859 7747

## Testing

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build
```

## License

This software is licensed under the MIT License. See the LICENSE file for details.
