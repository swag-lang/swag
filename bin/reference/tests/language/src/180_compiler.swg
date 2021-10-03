// #assert is a static assert (at compile time)
#assert true
#assert(true, "message")

// #defined(SYMBOL) returns true, at compile time, if the given symbol exists
// in the current context
#assert !#defined(DOES_NOT_EXISTS)
#assert #defined(Global)
var Global = 0

// static #if/#else, with an expression that can be evaluated at compile time
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

// 'isThisDebug' is marked with 'ConstExpr', so it can be automatically evaluated
// at compile time
#[Swag.ConstExpr]
func isThisDebug() => true

#if isThisDebug() == false
{
    #assert(false, "this should not be called !")
}

// A more complicated #assert
#[Swag.ConstExpr]
func factorial(x: s32)->s32
{
    if x == 1 return 1
    return x * factorial(x - 1)
}

#assert factorial(4) == 24 // Evaluated at compile time