# Enigma V300 - Fluke Option Key Calculator

> Complete collection of Enigma V300 option key algorithm implementations

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python 3.8+](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)

## Overview

This repository contains **four complete implementations** of the Fluke option key calculator for NetTool and other Fluke network testing devices. All implementations provide the same functionality using different programming paradigms and languages.

## Quick Start

### Python CLI/SDK (Recommended)

Install the main Python package:
```bash
pip install .
enigma-v300 --help
```

Generate a NetTool key:
```bash
enigma-v300 -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

Generate a key for other Fluke products:
```bash
enigma-v300 -e 0000607 7 6963
# Output: Option Key: 6406 2579 4859 7747
```

## Available Implementations

This repository includes **4 standalone implementations** of the Enigma V300 algorithms:

| Implementation | Location | Description | Best For |
|----------------|----------|-------------|----------|
| **Python Functions** | `python_functions/` | Standalone functional Python script | Functional programming enthusiasts, simple scripts |
| **Python Classes** | `python_classes/` | Standalone OOP Python script | OOP enthusiasts, educational purposes |
| **C** | `c_implementation/` | Pure C11 implementation | Embedded systems, portability, performance |
| **C++** | `cpp_implementation/` | Modern C++20 implementation | High-performance applications, C++ projects |

### Python Package (Main SDK)

The primary Python package with full CLI and library API support.

**Installation:**
```bash
pip install .
```

**CLI Usage:**
```bash
enigma-v300 -n 0003333016 4                    # Generate NetTool key
enigma-v300 -x 5dabade112dd 0003333016         # Validate NetTool key
enigma-v300 -e 0000607 7 6963                  # Generate EtherScope key
enigma-v300 -d 6406257948597747                # Decrypt key
```

**Library API:**
```python
from enigma_v300.core import EnigmaMenu, EnigmaC, Enigma2C
from enigma_v300 import functions

# Object-oriented approach
menu = EnigmaMenu()
menu.calculate_nettool_option_key("0003333016", 4)

# Functional approach
key = functions.generate_nettool_option_key("0003333016", 4)
print(f"Key: {functions.format_option_key(key)}")
```

### Python Functional Implementation

Standalone script using pure functions (no classes).

**Location:** `python_functions/`

**Usage:**
```bash
cd python_functions
./enigma_v300_functions.py -n 0003333016 4
```

See [python_functions/README.md](python_functions/README.md) for details.

### Python Class-Based Implementation

Standalone script using object-oriented programming.

**Location:** `python_classes/`

**Usage:**
```bash
cd python_classes
./enigma_v300_classes.py -n 0003333016 4
```

See [python_classes/README.md](python_classes/README.md) for details.

### C Implementation

Pure C11 implementation for maximum portability and performance.

**Location:** `c_implementation/`

**Build:**
```bash
cd c_implementation
cmake -S . -B build
cmake --build build
```

**Usage:**
```bash
./build/enigma_v300_pure_c -n 0003333016 4
```

**Quick compile:**
```bash
cd c_implementation
gcc -std=c11 src/main.c src/enigma_v300_pure_c.c -o enigma
./enigma -n 0003333016 4
```

See [c_implementation/README.md](c_implementation/README.md) for details.

### C++ Implementation

Modern C++20 implementation with namespaces and STL.

**Location:** `cpp_implementation/`

**Build:**
```bash
cd cpp_implementation
cmake -S . -B build
cmake --build build
```

**Usage:**
```bash
./build/enigma_v300_pure_cpp -n 0003333016 4
```

**Quick compile:**
```bash
cd cpp_implementation
g++ -std=c++20 src/enigma_v300_pure_cpp.cpp -o enigma
./enigma -n 0003333016 4
```

See [cpp_implementation/README.md](cpp_implementation/README.md) for details.

## Commands Reference

All implementations support these commands:

- `-n / --nettool SERIAL OPTION` - Generate NetTool keys (10-digit serial, numeric option)
- `-x / --check-nettool KEY SERIAL` - Validate a NetTool key
- `-e / --encrypt SERIAL OPTION PRODUCT` - Create keys for other products (7-digit serial, 3-digit option, product code)
- `-d / --decrypt KEY` - Decode a key and print the embedded serial/option/product information
- `-h / --help` - Display help information
- `-V / --version` - Display version information

Python implementations also support:
- `--verbose` - Enable debug logging
- `--logfile FILE` - Log to file (rotates at 10MB)

## Supported Products

- **3001** - NetTool Series II
- **7001** - LinkRunner Pro Duo
- **6963** - EtherScope/MetroScope
- **6964** - OneTouch AT
- **2186** - OptiView XG
- **1890** - ClearSight Analyzer
- **1895** - iClearSight Analyzer

## Test Cases

All implementations produce identical outputs for these test cases:

**NetTool (Serial: 0003333016, Option: 4):**
```
Expected: 5dab ade1 12dd
```

**EtherScope/MetroScope (Serial: 0000607, Option: 7, Product: 6963):**
```
Expected: 6406 2579 4859 7747
```

## Development

### Python Development

```bash
python -m venv .venv && source .venv/bin/activate
pip install -e .[test]
python -m pytest
nox -s tests  # Run full CI locally
```

### C/C++ Development

Each C/C++ implementation has its own build system (CMake) with testing support. See individual README files in `extras/` directories.

## Project Structure

```
enigma_v300/
├── python_functions/         # Standalone Python functional script
│   ├── enigma_v300_functions.py
│   └── README.md
├── python_classes/           # Standalone Python OOP script
│   ├── enigma_v300_classes.py
│   └── README.md
├── c_implementation/         # C implementation
│   ├── src/
│   │   ├── main.c
│   │   ├── enigma_v300_pure_c.c
│   │   └── enigma_v300_pure_c.h
│   ├── CMakeLists.txt
│   └── README.md
├── cpp_implementation/       # C++ implementation
│   ├── src/
│   │   ├── enigma_v300_pure_cpp.cpp
│   │   └── include/
│   ├── CMakeLists.txt
│   └── README.md
├── src/enigma_v300/          # Optional: Main Python package/SDK
│   ├── core.py               # Class-based implementation
│   ├── functions.py          # Functional implementation
│   ├── cli.py                # Command-line interface
│   └── __init__.py
├── tests/                    # Python tests
├── examples/                 # Example scripts
├── pyproject.toml            # Python package configuration
└── README.md                 # This file
```

## Algorithms

This project implements two ciphers:

### EnigmaC (NetTool)
- Used for NetTool devices with 10-digit serial numbers
- Hex-based encryption with XOR and rotor substitution
- Produces 12-character hex keys

### Enigma2C (Other Fluke Products)
- Used for other Fluke devices with 7-digit serial numbers
- Alphanumeric encryption with dual rotors (base-10 and base-26)
- Produces 16-character alphanumeric keys
- Includes checksum validation

## Version History

- **3.0.1** - Added standalone Python functional and class-based implementations
- **3.0.0** - Initial unified release with Python SDK, C, and C++ implementations

## License

This software is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author

Kris Armstrong

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.
