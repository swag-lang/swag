/**
The attribute '#[Swag.ConstExpr]' can be used to mark functions. It tells the compiler that this specific function can be called compile-time if possible.
*/

// 'isThisDebug' is marked with 'Swag.ConstExpr', so it can be automatically evaluated
// at compile time
#[Swag.ConstExpr]
func isThisDebug() => true

// This call is valid, and will be done by the compiler. As 'isThisDebug' returns
// false, then the '#error' inside the '#if' will never be compiled.
#if isThisDebug() == false
{
    #error "this should not be called !"
}

/**
A more complicated compile-time `#assert`.
*/
#[Swag.ConstExpr]
func factorial(x: s32)->s32
{
    if x == 1 return 1
    return x * factorial(x - 1)
}

#assert factorial(4) == 24 // Evaluated at compile time