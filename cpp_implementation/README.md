# Enigma v3.0.0 - C++ Implementation

## Overview

This is a C++20 implementation of the Fluke option key calculator. It generates and validates option keys for Fluke
network testing devices using EnigmaC (NetTool, 10-digit serial) and Enigma2C (other products, 7-digit serial) ciphers,
organized in namespaces for clarity.

- **Author**: Kris Armstrong
- **Version**: 3.0.0
- **License**: MIT

## Features

- Generates option keys for NetTool (10-digit serial) and other Fluke products (7-digit serial).
- Validates option keys against serial numbers and options.
- Interactive menu with product and option selection.
- Command-line support for automation.

## Requirements

- C++20 compatible compiler (e.g., g++ 10+, clang++ 11+, MSVC 2019+)
- Standard C++ library

## Compilation

```bash
cmake -S . -B build
cmake --build build
```

Or compile directly:
```bash
g++ -std=c++20 src/enigma_v300_pure_cpp.cpp -o enigma
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
```

## Test Cases

1. NetTool:

``` bash
./enigma -n 0003333016 4
```

* Output: Option Key: 5dab ade1 12dd

2. EtherScope:

```bash
./enigma -e 0000607 7 6963
```

* Output: Option Key: 6406 2579 4859 7747

## License

This software is licensed under the MIT License. See the LICENSE file for details.