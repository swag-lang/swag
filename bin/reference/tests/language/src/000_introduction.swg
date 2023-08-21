/**
The `swag-lang/swag/bin/reference/language` module provides a brief explanation of the language basic syntax and usage without the need of the [Swag standard modules](std.php) (`Std`). This documentation has been generated directly from the source tree of that module.

More advanced features such as dynamic arrays, dynamic strings or hash maps can be found in the [Std.Core](std.core.php) module and will not be covered in the examples to come. For now we will focus on the language itself.

As `reference/language` is written as a test module, you can run it with :
```
swag test --workspace:c:/swag-lang/swag/bin/reference
swag test -w:c:/swag-lang/swag/bin/reference
```
This will run all the test modules of the specified workspace (including this one). You can also omit the `--workspace` parameter (or `-w` in its short form) if you run Swag directly from the workspace folder.

Note that if you want to compile and run a single module in the workspace, you can specify it with the `--module` (`-m`) parameter.
```
swag test -w:c:/swag-lang/swag/bin/reference -m:test_language
```
*/