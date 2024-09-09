/**
The `swag-lang/swag/bin/reference/language` module offers a foundational introduction to the syntax and usage of the Swag programming language, separate from the [Swag standard modules](std.php) (`Std`). This documentation is auto-generated from the module's source code.

For more advanced features, such as dynamic arrays, dynamic strings, or hash maps, please consult the [Std.Core](std.core.php) module documentation, as these topics are beyond the scope of the examples covered here. This guide focuses exclusively on the core elements of the Swag language.

Since `reference/language` is implemented as a test module, you can run it using the following commands:
```
swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference
```
These commands will execute all test modules within the specified workspace, including this one. If you are running Swag from the workspace directory, the `--workspace` flag (or shorthand `-w`) can be omitted.

To compile and execute a specific module within the workspace, use the `--module` (or `-m`) flag:
```
swag test -w:c:/swag-lang/swag/bin/reference -m:test_language
```
*/