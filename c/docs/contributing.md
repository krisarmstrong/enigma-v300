# Contributing to Enigma V300 Pure C

## Getting Started

1. Fork the repository
2. Create a new branch for your feature or fix
3. Make your changes
4. Ensure all tests pass
5. Submit a pull request

## Code Style

- Follow standard C conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and modular

## Testing

All changes must include appropriate tests:

```bash
cd build
ctest
```

## Building

Use CMake to build the project:

```bash
cmake -B build
cd build
make
```

## Commit Messages

Use clear, descriptive commit messages:
- Start with a verb (Add, Fix, Update, etc.)
- Be specific about what changed
- Reference any related issues

## Pull Request Process

1. Ensure your code builds without warnings
2. All tests must pass
3. Update documentation if needed
4. Request review from maintainers

## Reporting Issues

Please provide:
- Clear description of the issue
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, compiler version, etc.)

## Questions

Feel free to open an issue for questions or discussions about the project.
