# Contributing to Enigma v3.0.0

Thank you for your interest in contributing to the Enigma v3.0.0 project! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Making Changes](#making-changes)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Reporting Bugs](#reporting-bugs)
- [Feature Requests](#feature-requests)

## Code of Conduct

This project adheres to a code of professional conduct. By participating, you are expected to:

- Be respectful and constructive in all interactions
- Focus on what is best for the project and community
- Show empathy towards other contributors
- Accept constructive criticism gracefully
- Avoid any form of harassment or discrimination

## Getting Started

### Prerequisites

- C++20 compatible compiler (g++ 10+, clang++ 11+, or MSVC 2019+)
- CMake 3.15 or higher (if using CMake build)
- Git for version control
- Basic understanding of cryptographic concepts

### Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/yourusername/enigma_v300_pure_cpp.git
   cd enigma_v300_pure_cpp
   ```

3. Add the upstream repository:
   ```bash
   git remote add upstream https://github.com/original/enigma_v300_pure_cpp.git
   ```

## Development Setup

### Building the Project

**Using g++:**
```bash
g++ -std=c++20 enigma_v300_pure_cpp.cpp -o enigma
```

**Using CMake:**
```bash
mkdir build
cd build
cmake ..
make
```

**Using clang++:**
```bash
clang++ -std=c++20 enigma_v300_pure_cpp.cpp -o enigma
```

### Testing Your Build

Run the test cases from the README:

```bash
# NetTool test
./enigma -n 0003333016 4
# Expected: 5dab ade1 12dd

# EtherScope test
./enigma -e 0000607 7 6963
# Expected: 6406 2579 4859 7747
```

## Coding Standards

### C++ Style Guidelines

#### Naming Conventions

- **Constants**: `UPPER_SNAKE_CASE` (e.g., `PRODUCT_CODE_SIZE`)
- **Functions**: `snake_case` (e.g., `enigma_c_encrypt`)
- **Structs**: `PascalCase` (e.g., `ProductInfo`)
- **Variables**: `snake_case` (e.g., `serial_number`)
- **Struct members**: `snake_case` (e.g., `product_code`)

#### Code Formatting

- **Indentation**: 4 spaces (no tabs)
- **Braces**: Opening brace on same line for functions, structs, and control structures
- **Line length**: Prefer 120 characters maximum
- **Comments**: Use `//` for single-line, keep comments current with code

#### C++ Standards

- Use C++20 features where appropriate
- Prefer `constexpr` for compile-time constants
- Use `std::string` over C-style strings
- Use references for output parameters
- Use `const` for read-only parameters
- Avoid raw pointers; prefer references or smart pointers

#### Example:

```cpp
void enigma_c_encrypt(const std::string& input_key, std::string& output_key, size_t len)
{
    output_key.resize(len);
    for (size_t index = 0; index < len; ++index)
    {
        // Processing logic here
    }
}
```

### Documentation

- Add comments for complex algorithms
- Document all public functions with parameters and return values
- Update README.md if adding new features
- Keep docs/ directory synchronized with code changes

## Making Changes

### Creating a Branch

Create a descriptive branch name:

```bash
git checkout -b feature/add-new-product-support
git checkout -b fix/serial-validation-bug
git checkout -b docs/update-api-documentation
```

### Commit Messages

Write clear, concise commit messages:

```
Add support for new LinkRunner Pro Duo products

- Added product codes 7002, 7003 to PRODUCT_TABLE
- Updated PRODUCT_OPTIONS with new option codes
- Added validation tests for new products
```

**Format:**
- First line: Brief summary (50 chars or less)
- Blank line
- Detailed description with bullet points if needed

### Code Changes

1. Make your changes in logical, atomic commits
2. Ensure code compiles without warnings
3. Test all affected functionality
4. Update documentation as needed
5. Follow the established coding style

## Testing

### Manual Testing

Test all modifications thoroughly:

1. **Compilation test**: Ensure code compiles cleanly
   ```bash
   g++ -std=c++20 -Wall -Wextra enigma_v300_pure_cpp.cpp -o enigma
   ```

2. **Functional tests**: Run known test cases
3. **Interactive testing**: Test menu navigation
4. **Edge cases**: Test boundary conditions and error handling

### Adding Test Cases

When adding new features, include test cases in your pull request:

- Document expected input and output
- Include both positive and negative test cases
- Test boundary conditions
- Verify error handling

**Example:**
```
Test Case: NetTool with maximum serial number
Input: -n 9999999999 9
Expected Output: [calculated key]
Status: PASS
```

## Submitting Changes

### Pull Request Process

1. **Update your branch:**
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Push your changes:**
   ```bash
   git push origin feature/your-feature-name
   ```

3. **Create Pull Request:**
   - Go to GitHub and create a pull request
   - Use a clear, descriptive title
   - Reference any related issues (#123)
   - Describe your changes in detail
   - List any breaking changes
   - Include test results

### Pull Request Template

```markdown
## Description
[Describe your changes]

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Code refactoring

## Testing
[Describe testing performed]

## Checklist
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Coding standards followed
- [ ] Commit messages are clear
```

### Review Process

- Maintainer will review your pull request
- Address any feedback or requested changes
- Once approved, your changes will be merged

## Reporting Bugs

### Before Submitting

1. Check existing issues to avoid duplicates
2. Verify the bug exists in the latest version
3. Collect relevant information

### Bug Report Template

```markdown
## Description
[Clear description of the bug]

## Steps to Reproduce
1. [First step]
2. [Second step]
3. [etc.]

## Expected Behavior
[What should happen]

## Actual Behavior
[What actually happens]

## Environment
- OS: [e.g., Ubuntu 22.04, Windows 11, macOS 13]
- Compiler: [e.g., g++ 11.2, clang++ 13.0]
- Version: [e.g., 3.0.0]

## Additional Context
[Any other relevant information]
```

## Feature Requests

We welcome feature suggestions! When requesting a feature:

1. Check existing issues/requests
2. Clearly describe the feature and use case
3. Explain why it would be valuable
4. Provide examples if applicable

### Feature Request Template

```markdown
## Feature Description
[Clear description of the feature]

## Use Case
[Why is this feature needed?]

## Proposed Solution
[How should it work?]

## Alternatives Considered
[Other approaches you've thought about]

## Additional Context
[Any other relevant information]
```

## Areas for Contribution

Here are some areas where contributions are especially welcome:

### High Priority

- Unit test framework integration
- Additional product/option support
- Performance optimizations
- Cross-platform compatibility improvements

### Medium Priority

- Enhanced error messages
- Logging capabilities
- Configuration file support
- Batch processing mode

### Low Priority

- GUI interface
- Web service API
- Additional encryption algorithms
- Internationalization support

## Getting Help

If you need help:

- Check existing documentation in docs/
- Review closed issues for similar problems
- Open a new issue with the "question" label
- Contact the maintainer: Kris Armstrong

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Recognition

Contributors will be recognized in:
- CHANGELOG.md for significant contributions
- README.md contributors section
- Git commit history

Thank you for contributing to Enigma v3.0.0!
