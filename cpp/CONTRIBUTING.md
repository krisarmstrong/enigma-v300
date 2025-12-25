# Contributing to Enigma v3.0.0

Thank you for your interest in contributing! Please read our detailed contribution guidelines.

## Quick Start

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes following our coding standards
4. Test thoroughly
5. Commit with clear messages
6. Push and create a pull request

## Coding Standards

- Follow C++20 standards
- Use 4-space indentation
- Name functions in `snake_case`
- Name constants in `UPPER_SNAKE_CASE`
- Add comments for complex logic
- Ensure code compiles without warnings

## Testing

Before submitting:

```bash
# Compile with warnings
g++ -std=c++20 -Wall -Wextra enigma_v300_pure_cpp.cpp -o enigma

# Run test cases
./enigma -n 0003333016 4        # Expected: 5dab ade1 12dd
./enigma -e 0000607 7 6963      # Expected: 6406 2579 4859 7747
```

## Pull Request Process

1. Update documentation for any API changes
2. Add test cases for new features
3. Ensure all tests pass
4. Reference any related issues
5. Provide clear description of changes

## Reporting Issues

- Use the issue tracker for bugs and feature requests
- Provide reproduction steps for bugs
- Include environment details (OS, compiler version)

## Detailed Guidelines

For complete contribution guidelines, see [docs/contributing.md](docs/contributing.md).

## Code of Conduct

- Be respectful and constructive
- Focus on what's best for the project
- Accept feedback gracefully

## Questions?

Open an issue with the "question" label or contact the maintainer.

## License

Contributions are licensed under the MIT License.

---

**Author:** Kris Armstrong
