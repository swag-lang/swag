/**
The `swag-lang/swag/bin/reference/language` module provides an introductory overview of the basic syntax and usage of the Swag programming language, independent of the [Swag standard modules](std.php) (`Std`). This documentation has been auto-generated directly from the source code of the module.

For more advanced features like dynamic arrays, dynamic strings, or hash maps, please refer to the [Std.Core](std.core.php) module documentation, as these will not be covered in the upcoming examples. This guide will focus solely on the foundational elements of the Swag language.

As `reference/language` is implemented as a test module, it can be executed with the following commands:
```
swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference
```
These commands will execute all test modules within the specified workspace (including this one). If you are running Swag directly from the workspace directory, the `--workspace` parameter (or its shorthand `-w`) can be omitted.

Additionally, if you wish to compile and execute a specific module within the workspace, you can do so by specifying the module with the `--module` (`-m`) parameter:
```
swag test -w:c:/swag-lang/swag/bin/reference -m:test_language
```
*/
