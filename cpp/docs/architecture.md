# Architecture Documentation

## Overview

Enigma v3.0.0 is a C++20 implementation of the Fluke option key calculator. The project provides cryptographic key generation and validation for Fluke network testing devices using two distinct encryption algorithms: EnigmaC and Enigma2C.

## System Architecture

### Core Components

#### 1. Encryption Engines

**EnigmaC Engine**
- Target: NetTool devices (10-digit serial numbers)
- Algorithm: Rotor-based XOR encryption with hex output
- Key Functions:
  - `enigma_c_encrypt()`: Encrypts input key using 16-element rotor
  - `enigma_c_decrypt()`: Decrypts and validates option keys
  - `enigma_c_check_option_key()`: Validates key against serial and option

**Enigma2C Engine**
- Target: Other Fluke products (7-digit serial numbers)
- Algorithm: Checksum-based rotor encryption with alphanumeric output
- Key Functions:
  - `enigma2_c_encrypt()`: Encrypts with checksum and dual rotor system
  - `enigma2_c_decrypt()`: Decrypts and validates checksum
  - `enigma2_c_check_option_key()`: Validates option key

#### 2. User Interface Layer

**Interactive Menu System**
- Main menu navigation with 4 primary operations
- Product code selection with predefined database
- Option code selection per product
- Custom code entry support
- Input validation and error handling

**Command-Line Interface**
- Supports batch operation with flags
- Flags: `-n` (NetTool), `-x` (check NetTool), `-e` (EtherScope), `-l` (LinkRunner), `-d` (decrypt)
- Positional arguments for serial numbers, option codes, and product codes

#### 3. Data Layer

**Product Database**
- `PRODUCT_TABLE`: Maps product codes to names and abbreviations
- `PRODUCT_OPTIONS`: Links products to their available option codes
- Supports 7 product families with multiple options each

**Rotor Tables**
- `ENIGMA_C_ROTOR`: 16-element permutation array for EnigmaC
- `ENIGMA2_E_ROTOR_10/26`: Encryption rotors for digits and letters
- `ENIGMA2_D_ROTOR_10/26`: Decryption rotors for digits and letters

## Data Flow

### Key Generation Flow

```
User Input → Serial Number Validation → Product/Option Selection →
  → Key Assembly → Encryption Engine → Formatted Output
```

### Key Validation Flow

```
User Input → Key Format Validation → Decryption Engine →
  → Checksum Verification → Component Extraction → Validation Result
```

## Design Patterns

### Modular Encryption

The system uses separate encryption/decryption function pairs for each algorithm, allowing:
- Independent algorithm evolution
- Clear separation of concerns
- Easy testing and maintenance

### Menu-Driven Navigation

Hierarchical menu system provides:
- Progressive disclosure of options
- Context-sensitive choices
- Graceful error recovery

### Constant-Based Configuration

Extensive use of `constexpr` constants for:
- Type safety
- Compile-time optimization
- Easy configuration management

## Security Considerations

### Input Validation

- All serial numbers validated for length and digit content
- Option keys validated for format and character set
- Product/option codes checked against allowed values

### Encryption Properties

- EnigmaC: Position-dependent XOR with rotor transformation
- Enigma2C: Checksum-based validation with dual rotor system
- Both algorithms include anti-tampering checks

## Build System

- CMake-based build configuration
- C++20 standard requirement
- Platform-independent design
- Debug build support via cmake-build-debug

## Error Handling

- Input validation with user-friendly error messages
- Graceful failure for invalid keys or serial numbers
- Exit codes for command-line operation
- Exception handling for numeric conversions

## Extensibility

### Adding New Products

1. Add entry to `PRODUCT_TABLE`
2. Define options in `PRODUCT_OPTIONS`
3. No code changes required

### Algorithm Variants

New encryption algorithms can be added by:
1. Implementing encrypt/decrypt function pairs
2. Adding validation function
3. Extending menu system for selection

## Performance Characteristics

- O(n) encryption/decryption where n is key length
- Constant-time lookups for product/option information
- Minimal memory footprint (stack-based operation)
- No dynamic allocation in core algorithms

## Testing Strategy

- Manual test cases documented in README
- Command-line automation support for regression testing
- Interactive testing via menu system
- Known good keys for validation

## Future Enhancements

- API library extraction for integration
- Web service interface
- Batch processing mode
- Enhanced logging capabilities
