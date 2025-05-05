/**
# `#assert`
The `#assert` directive is used to perform a static assertion during the compilation process.
It ensures that a particular condition is true at compile time. If the condition evaluates to `false`,
compilation will fail, providing an error message. This is particularly useful for enforcing
compile-time invariants and validating assumptions within your code.
*/
#assert(true) // This assertion always passes, so no error is triggered.

/**
# `#defined(SYMBOL)`
The `#defined(SYMBOL)` intrinsic checks if a given symbol exists within the current context
at compile time. It returns `true` if the symbol is defined, and `false` otherwise. This is
useful for conditional compilation, allowing you to verify the existence of variables,
constants, or functions before using them.
*/
#assert(!#defined(DOES_NOT_EXISTS)) // Ensures that the symbol 'DOES_NOT_EXISTS' is not defined.
#assert(#defined(Global))            // Confirms that the symbol 'Global' is defined.
var Global = 0                      // Define a global variable 'Global'.

/**
# `#if`/`#elif`/`#else`
The `#if`/`#elif`/`#else` directives are used for static conditional compilation.
They evaluate expressions at compile time and include or exclude code based on the result.
This mechanism allows you to compile different sections of code based on predefined constants
or compile-time conditions.
*/
const DEBUG   = 1
const RELEASE = 0

#if DEBUG
{
    // This block is compiled because DEBUG is set to 1.
}
#elif RELEASE
{
    // This block would be compiled if RELEASE were true and DEBUG were false.
}
#else
{
    // This block is compiled if neither DEBUG nor RELEASE is true.
}

/**
# `#error`/`#warning`
The `#error` and `#warning` directives allow you to raise compile-time errors and warnings,
respectively. `#error` will cause the compilation to fail with a custom error message,
while `#warning` will produce a warning message during compilation but will not stop the process.
These directives are useful for enforcing compile-time checks and providing informative messages
during the build process.
*/
#if false
{
    #error "this is an error"       // Raises a compile-time error if this block is reached.
    #warning "this is a warning"    // Raises a compile-time warning if this block is reached.
}

/**
# `#global`
The `#global` directive can be placed at the top of a source file to apply global settings
or attributes across the entire file. These directives control various aspects of the
compilation and symbol visibility for the entire file.

Examples:
```swag
// Skip the content of the file (but it must be a valid Swag file).
#global skip

// All symbols in the file will be public (accessible from other modules).
#global public

// All symbols in the file will be internal (accessible only within the same module).
#global internal

// All symbols in the file will be placed within the namespace 'Toto'.
#global namespace Toto

// Conditional compilation for the entire file.
#global #if DEBUG == true

// Apply attributes to all declarations in the file.
#global #[Swag.Safety("", true)]

// Export the entire file for external usage.
// This is similar to making everything public, but the file will also be copied
// in its entirety to the public folder.
#global export
```
*/

/**
# `#foreignlib`
The `#foreignlib` directive is used to link with an external library during the compilation
process. This allows your program to utilize functions, variables, and resources defined
in the external library. The library name should be provided as a string.

Example:
```swag
#foreignlib "windows.lib"
```
This example links the program with the "windows.lib" library, allowing the use of Windows
API functions and resources defined within that library.
*/
