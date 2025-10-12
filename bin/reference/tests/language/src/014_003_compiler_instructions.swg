/**
# '#assert'
The '#assert' directive performs a static assertion during compilation.
If the condition is false, compilation fails with an error message.
Use it to enforce compile-time invariants and validate assumptions.
*/
#assert(true) // Always passes; no error.

/**
# '#defined(SYMBOL)'
'#defined(SYMBOL)' checks at compile time whether a symbol exists in the
current context. It returns true if defined, false otherwise. Useful for
conditional compilation before referencing variables, constants, or functions.
*/
#assert(!#defined(DOES_NOT_EXISTS)) // Ensure the symbol is not defined.
#assert(#defined(Global)) // Confirms 'Global' is defined.
var Global = 0 // Define a global variable 'Global'.

/**
# '#if'/'#elif'/'#else'
Static conditional compilation. Expressions are evaluated at compile time to
include or exclude code based on constants or conditions.
*/
const DEBUG   = 1
const RELEASE = 0

#if DEBUG
{
    // Compiled because DEBUG == 1.
}
#elif RELEASE
{
    // Would compile if RELEASE were true and DEBUG were false.
}
#else
{
    // Compiled if neither DEBUG nor RELEASE is true.
}

/**
# '#error'/'#warning'
'#error' raises a compile-time error with a custom message.
'#warning' emits a compile-time warning without stopping compilation.
Useful for enforcing checks and surfacing build-time information.
*/
#if false
{
    #error("this is an error") // Compile-time error (if reached).
    #warning("this is a warning") // Compile-time warning (if reached).
}

/**
# '#global'
Place '#global' at the top of a source file to apply global settings or
attributes across the entire file. Controls compilation and symbol visibility.

Examples (write these as top-level directives):
  #global skip                        // Skip file content (file must still be valid).
  #global public                      // All symbols become public.
  #global internal                    // All symbols are internal to the module.
  #global namespace Toto              // Place all symbols in namespace 'Toto'.
  #global #if DEBUG == true           // Conditional compilation for the file.
  #global #[Swag.Safety('', true)]    // Apply an attribute to all declarations.
  #global export                      // Export the file; copied to the public folder.
*/

/**
# '#foreignlib'
Link against an external library during compilation to use its functions,
variables, or resources. Provide the library name as a string.

Example:
  #foreignlib("windows.lib")
This links the program with "windows.lib" and enables Windows API usage.
*/
