/**
# #assert
`#assert` is a static assert (at compile time).
*/
#assert true

/**
`@defined(SYMBOL)` returns true, at compile time, if the given symbol exists in the current context.
*/
#assert !@defined(DOES_NOT_EXISTS)
#assert @defined(Global)
var Global = 0

/**
# #if/#else
A static `#if/#elif/#else`, with an expression that can be evaluated at compile time.
*/
const DEBUG   = 1
const RELEASE = 0
#if DEBUG
{
}
#elif RELEASE
{
}
#else
{
}

/**
# #error/#warning
`#error` to raise a compile-time error, and `#warning` to raise a compile-time warning.
*/
#if false
{
    #error "this is an error"
    #warning "this is a warning"
}

/**
# #global
A bunch of `#global` can be put **at the top** of a source file.

```swag
// Skip the content of the file (but must be a valid swag file)
#global skip

// All symbols in the file will be public/internal
#global public
#global internal

// All symbols in the file will go in the namespace 'Toto'
#global namespace Toto

// A #if for the whole file
#global #if DEBUG == true

// Some attributes can be assigned to the full file
#global #[Swag.Safety("", true)]

// The file will be exported for external usage
// It's like putting everything in public, except that the file will
// be copied in its totality in the public folder
#global export
```
*/

/**
# #foreignlib
Link with a given external library.
```swag
#foreignlib "windows.lib"
```
*/
