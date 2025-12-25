# Enigma V300 Pure C - Architecture

## Overview

This project is a pure C implementation of the Enigma V300 encryption machine. It provides a complete simulation of the historical Enigma machine with support for various rotor configurations and settings.

## Project Structure

- `src/enigma_v300_pure_c.h`: Header file with core function declarations and data structures
- `src/enigma_v300_pure_c.c`: Main implementation file containing all Enigma machine logic
- `src/main.c`: Entry point for the application
- `tests/`: Test suite for validation
- `CMakeLists.txt`: Build configuration

## Key Components

### Core Machine Simulation
- Rotor implementation and rotation logic
- Reflector configuration
- Plugboard (Stecker) wiring
- Message encoding/decoding process

### Build System
- CMake for cross-platform compilation
- Support for multiple compiler configurations

## Dependencies

- Standard C library
- CMake 3.0 or higher for building

## Compilation

```bash
cmake -B build
cd build
make
```

## Usage

Run the compiled executable to process Enigma messages:
```bash
./enigma_v300_pure_c
```

Refer to README.md for detailed usage instructions.
