# Contributing to Enigma V300

Thank you for your interest in contributing to the Enigma V300 project!

## Code of Conduct

This project follows standard open source etiquette. Be respectful, constructive, and helpful.

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Which implementation(s) are affected

### Suggesting Enhancements

Open an issue describing:
- The enhancement you'd like to see
- Why it would be useful
- Which implementation(s) it would affect

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Make your changes**
   - Ensure all 4 implementations remain consistent
   - Update documentation as needed
   - Add tests if applicable
4. **Test your changes**
   - Run tests for affected implementations
   - Verify output matches expected results
5. **Commit your changes** (`git commit -m 'Add amazing feature'`)
6. **Push to your branch** (`git push origin feature/amazing-feature`)
7. **Open a Pull Request**

## Development Guidelines

### Maintaining Consistency

All 4 implementations must:
- Produce **identical output** for the same inputs
- Support the same command-line arguments
- Follow the same algorithm logic
- Pass all CI/CD tests

### Code Style

**Python:**
- Follow PEP 8
- Use type hints where appropriate
- Include docstrings for functions

**C:**
- Follow C11 standard
- Keep code portable (avoid platform-specific code)
- Comment complex algorithms

**C++:**
- Follow modern C++20 practices
- Use STL where appropriate
- Prefer RAII and smart pointers

### Testing

Before submitting a PR:

1. **Test locally:**
   ```bash
   # Python
   cd python_functions && ./enigma_v300_functions.py -n 0003333016 4
   cd python_classes && ./enigma_v300_classes.py -n 0003333016 4

   # C
   cd c_implementation
   cmake -S . -B build -DBUILD_TESTING=ON
   cmake --build build
   ctest --test-dir build

   # C++
   cd cpp_implementation
   cmake -S . -B build
   cmake --build build
   ./build/enigma_v300_pure_cpp -n 0003333016 4
   ```

2. **Verify output consistency:**
   All implementations must produce:
   - NetTool (0003333016, 4): `5dab ade1 12dd`
   - EtherScope (0000607, 7, 6963): `6406 2579 4859 7747`

3. **Check CI/CD:**
   GitHub Actions will automatically run on your PR

### Documentation

- Update README.md if adding features
- Update individual implementation READMEs if needed
- Add code comments for complex logic
- Update this CONTRIBUTING.md if changing development process

## Questions?

Open an issue or start a discussion. We're happy to help!

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
