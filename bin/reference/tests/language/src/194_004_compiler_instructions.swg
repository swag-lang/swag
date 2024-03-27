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
