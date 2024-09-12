/**
The `swag-lang/swag/bin/reference/language` module provides a foundational introduction to the syntax and essential features of the Swag programming language. It is designed to offer insight into core language elements, separate from the more extensive [Swag standard modules](std.php) (`Std`).

# Scope

This documentation is auto-generated directly from the module’s source code and serves as a basic guide to Swag’s syntax and functionality. Advanced topics such as dynamic arrays, dynamic strings, and hash maps are covered in the [Std.Core](std.core.php) module documentation, which you should reference for more sophisticated implementations beyond the examples covered here.

# Running the Test Module

The `reference/language` module is implemented as a test module to demonstrate the language’s core features in action. You can run the module by executing the following commands from the terminal:

```bash
swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference
```

These commands execute all test modules within the specified workspace, including `reference/language`. If you are already working within the workspace directory, the `--workspace` flag (or shorthand `-w`) can be omitted.

# Compiling and Running a Specific Test Module

To compile and execute a specific test module, such as `test_language`, use the `--module` (or `-m`) flag in conjunction with the `swag test` command:

```bash
swag test -w:c:/swag-lang/swag/bin/reference -m:test_language
```

This will target and run only the specified module, enabling you to focus on individual components for testing and development purposes.

---

# Key Features of the `reference/language` Module:

1. **Core Syntax**: Covers the fundamental elements of the Swag programming language, including variables, control structures, functions, and basic data types.
2. **Test-Driven**: All examples and modules are designed to be executable as tests, allowing developers to verify their understanding of core Swag concepts in real time.
3. **Auto-Generated Documentation**: This guide is generated directly from the source code, ensuring accuracy and up-to-date information on Swag's core functionality.

For further exploration of Swag’s advanced features, such as memory management or concurrency, consult the [Std.Core](std.core.php) documentation.
*/
