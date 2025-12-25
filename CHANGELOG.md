# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- CLI standardization across all four implementations (Python functions, Python classes, C, C++)
- New `-l / --linkrunner` option for generating LinkRunner Pro keys
- New `--list-products` option to display all known product codes
- New `--list-options CODE` option to display options for a specific product
- Comprehensive test suites for all implementations (38 Python tests, 20 C tests, 22 C++ tests)
- Test vectors verified against Enigma V200 reference implementation

### Changed
- Unified CLI interface: all implementations now support identical command-line options
- Updated README.md with correct directory paths and complete CLI documentation
- Improved help output formatting across all implementations

### Fixed
- Fixed `-l` option in C and C++ implementations to work non-interactively
- Fixed Python `logger.info(end="")` bug (logging module doesn't support `end` parameter)
- Fixed C++ help/version flags to work correctly with non-numeric arguments

### Removed
