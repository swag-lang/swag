// #assert is a static assert (at compile time)
#assert true
#assert(true, "message")

// #defined(SYMBOL) returns true, at compile time, if the given symbol exists
// in the current context
#assert !#defined(DOES_NOT_EXISTS)
#assert #defined(Global)
var Global = 0

// static #if/#else, with an expression that can be evaluated at compiler time
const DEBUG = 1
#if DEBUG
{
}
#else
{
}

// 'isThisDebug' is marked with 'constexpr', so it can be automatically evaluated
// at compile time
#[Swag.constexpr]
func isThisDebug() => true

#if isThisDebug() == false
{
    #assert(false, "this should not be called !")
}

#[Swag.constexpr]
func factorial(x: s32)->s32
{
    if x == 1 return 1
    return x * factorial(x - 1)
}

#assert factorial(4) == 24