# Enigma V300 - Fluke Option Key Calculator
[![Checks](https://github.com/krisarmstrong/enigma-v300/actions/workflows/checks.yml/badge.svg)](https://github.com/krisarmstrong/enigma-v300/actions/workflows/checks.yml)


> Complete collection of Enigma V300 option key algorithm implementations

[![Test All Implementations](https://github.com/krisarmstrong/enigma-v300/actions/workflows/test-all-implementations.yml/badge.svg)](https://github.com/krisarmstrong/enigma-v300/actions/workflows/test-all-implementations.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Python 3.8+](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)
[![C11](https://img.shields.io/badge/C-11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)

## Overview

This repository contains **four complete implementations** of the Fluke option key calculator for NetTool and other Fluke network testing devices. All implementations provide the same functionality using different programming paradigms and languages.

## Quick Start

Each implementation is standalone and produces identical output. Choose based on your language preference:

**Python (Functional):**
```bash
python python/enigma_v300_functions.py -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

**Python (Object-Oriented):**
```bash
python python/enigma_v300_classes.py -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

**C:**
```bash
cd c_implementation
cmake -S . -B build && cmake --build build
./build/enigma_v300_pure_c -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

**C++:**
```bash
cd cpp_implementation
cmake -S . -B build && cmake --build build
./build/enigma_v300_pure_cpp -n 0003333016 4
# Output: Option Key: 5dab ade1 12dd
```

## Available Implementations

This repository includes **4 standalone implementations** of the Enigma V300 algorithms:

| Implementation | Location | Description | Best For |
|----------------|----------|-------------|----------|
| **Python Functions** | `python/enigma_v300_functions.py` | Standalone functional Python script | Functional programming enthusiasts, simple scripts |
| **Python Classes** | `python/enigma_v300_classes.py` | Standalone OOP Python script | OOP enthusiasts, educational purposes |
| **C** | `c_implementation/` | Pure C11 implementation | Embedded systems, portability, performance |
| **C++** | `cpp_implementation/` | Modern C++20 implementation | High-performance applications, C++ projects |

### Python Functional Implementation

Standalone script using pure functions (no classes).

**Location:** `python/enigma_v300_functions.py`

**Usage:**
```bash
python python/enigma_v300_functions.py -n 0003333016 4
```

### Python Class-Based Implementation

Standalone script using object-oriented programming.

**Location:** `python/enigma_v300_classes.py`

**Usage:**
```bash
python python/enigma_v300_classes.py -n 0003333016 4
```

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

**Key Generation:**
- `-n / --nettool SERIAL OPTION` - Generate NetTool keys (10-digit serial, numeric option)
- `-e / --encrypt SERIAL OPTION PRODUCT` - Create keys for other products (7-digit serial, 3-digit option, product code)
- `-l / --linkrunner SERIAL OPTION` - Generate LinkRunner Pro keys (7-digit serial, option)

**Key Validation:**
- `-x / --check-nettool KEY SERIAL` - Validate a NetTool key
- `-d / --decrypt KEY` - Decode a key and print the embedded serial/option/product information

**Utility:**
- `-h / --help / -?` - Display help information
- `-V / --version` - Display version information
- `--list-products` - List all known product codes
- `--list-options CODE` - List available options for a product code

**Python-only options:**
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

## Development & Testing

### Continuous Integration

All 4 implementations are automatically tested on every push using GitHub Actions. The CI workflow:
- Tests each implementation independently
- Verifies all implementations produce identical output
- Runs C implementation unit tests
- Validates both test cases (NetTool and EtherScope)

### Running Tests Locally

**Python implementations:**
```bash
# Run pytest suite
pytest tests/test_enigma_v300.py -v

# Test Python Functions directly
python python/enigma_v300_functions.py -n 0003333016 4

# Test Python Classes directly
python python/enigma_v300_classes.py -n 0003333016 4
```

**C implementation:**
```bash
cd c_implementation
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

**C++ implementation:**
```bash
cd cpp_implementation
cmake -S . -B build
cmake --build build
./build/enigma_v300_pure_cpp -n 0003333016 4
./build/enigma_v300_pure_cpp -e 0000607 7 6963
```

## Project Structure

```
enigma-v300/
├── python/                   # Python implementations
│   ├── enigma_v300_functions.py  # Functional style
│   └── enigma_v300_classes.py    # OOP style
├── c_implementation/         # C11 implementation
│   ├── src/
│   │   └── enigma_v300_pure_c.c
│   ├── tests/
│   │   └── test_enigma_v300.sh
│   ├── CMakeLists.txt
│   └── README.md
├── cpp_implementation/       # C++20 implementation
│   ├── src/
│   │   └── enigma_v300_pure_cpp.cpp
│   ├── tests/
│   │   └── test_enigma_v300.sh
│   ├── CMakeLists.txt
│   └── README.md
├── tests/                    # Python pytest suite
│   └── test_enigma_v300.py
├── .github/
│   └── workflows/            # CI/CD pipelines
├── pyproject.toml            # Python project config
├── CHANGELOG.md
└── README.md
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

### Reporting Issues

Found a bug or have a feature request? Please use our [issue templates](.github/ISSUE_TEMPLATE/):
- [Bug Report](.github/ISSUE_TEMPLATE/bug_report.md)
- [Feature Request](.github/ISSUE_TEMPLATE/feature_request.md)

### Submitting Pull Requests

Please read our [Pull Request template](.github/pull_request_template.md) and ensure:
- All 4 implementations remain consistent
- Tests pass locally and in CI/CD
- Documentation is updated

## Repository Topics

`fluke` `nettool` `option-keys` `encryption` `enigma` `python` `c` `cpp` `cryptography` `network-testing` `ci-cd` `github-actions`

## Development

Run the full local checks:

```bash
./check.sh
```

