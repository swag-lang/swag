/**
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
A static `#if/#elif/#else`, with an expression that can be evaluated at compile time.
*/
const DEBUG = 1
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
`#error` to raise a compile time error, and #warning to raise a compile time warning.
*/
#if false
{
    #error   "this is an error"
    #warning "this is a warning"
}

// 'isThisDebug' is marked with 'Swag.ConstExpr', so it can be automatically evaluated
// at compile time
#[Swag.ConstExpr]
func isThisDebug() => true

// This call is valid
#if isThisDebug() == false
{
    #error "this should not be called !"
}

// This time 'isThisRelease' is not marked with 'Swag.ConstExpr'
func isThisRelease() => true

// But this call is still valid because we force the compile time execution with '#run'
#if #run isThisRelease() == false
{
    #error "this should not be called !"
}

/**
A more complicated `#assert`.
*/
#[Swag.ConstExpr]
func factorial(x: s32)->s32
{
    if x == 1 return 1
    return x * factorial(x - 1)
}

#assert factorial(4) == 24 // Evaluated at compile time