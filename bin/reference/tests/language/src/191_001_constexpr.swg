/**
# Compile-Time Function Evaluation with `#[Swag.ConstExpr]`

The attribute `#[Swag.ConstExpr]` is used to mark functions that can be evaluated at compile time. This tells the compiler that the function's result can be determined during compilation, rather than at runtime, if the necessary conditions are met.

This feature is particularly useful for functions that return constant values or perform operations that are known ahead of time, allowing the compiler to optimize the code by eliminating unnecessary runtime calculations.
*/

// The function 'isThisDebug' is marked with 'Swag.ConstExpr', meaning it can be
// evaluated at compile time. Since the function always returns `true`, this value
// is determined during compilation.
#[Swag.ConstExpr]
func isThisDebug() => true

// This conditional block demonstrates how 'isThisDebug' can be used in a compile-time
// context. Since 'isThisDebug' returns `true`, the condition `isThisDebug() == false`
// will evaluate to `false`, and the compiler will never compile the code inside the block.
#if isThisDebug() == false
{
    #error "this should not be called!"
}

/**
# Recursive Compile-Time Evaluation

The `#[Swag.ConstExpr]` attribute can also be applied to more complex functions, such as those that perform recursion. This allows such functions to be fully evaluated at compile time, reducing runtime overhead.
*/
#[Swag.ConstExpr]
func factorial(x: s32) -> s32
{
    // Base case: if x is 1, return 1
    if x == 1:
        return 1
    
    // Recursive case: multiply x by the factorial of (x - 1)
    return x * factorial(x - 1)
}

// The `#assert` directive is used here to ensure that the result of 'factorial(4)' is 24.
// Since 'factorial' is marked as 'Swag.ConstExpr', this calculation is performed during
// compilation, and the assertion is checked before the code is even run.
#assert factorial(4) == 24 // Evaluated at compile time

/**
# Compile-Time Constant Expressions

In this example, `#[Swag.ConstExpr]` is used to define a simple constant expression.
This function returns a fixed value, which the compiler can evaluate during compilation.
*/
#[Swag.ConstExpr]
func getMagicNumber() -> s32
{
    return 42
}

// Since 'getMagicNumber()' is a constant expression, this assertion is checked at compile time.
#assert getMagicNumber() == 42

/**
# Compile-Time Conditional Logic

Here, the function `isEven` checks if a given number is even. By marking it with
`#[Swag.ConstExpr]`, the compiler can evaluate this logic at compile time.
*/
#[Swag.ConstExpr]
func isEven(x: s32) -> bool
{
    return x % 2 == 0
}

// This block will only compile if the number 4 is even, which it is. So, the error
// will not be triggered, and the code compiles successfully.
#if isEven(4) == false
{
    #error "4 should be even!"
}

/**
# Compile-Time Slice Operations

In this example, we use `#[Swag.ConstExpr]` to calculate the sum of elements in an array.
*/
#[Swag.ConstExpr]
func arraySum(arr: const [..] s32) -> s32
{
    var sum = 0
    visit val in arr:
        sum += val
    return sum
}

// The sum of this array is computed at compile time, allowing the program to skip
// runtime calculation and simply use the precomputed result.
#assert arraySum([1, 2, 3, 4, 5]) == 15

/**
# Compile-Time Fibonacci Sequence

This example demonstrates how `#[Swag.ConstExpr]` can be used with a recursive function
to compute the Fibonacci sequence at compile time.
*/
#[Swag.ConstExpr]
func fibonacci(n: s32) -> s32
{
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)
}

// The 5th Fibonacci number is 5, and this value is computed at compile time.
#assert fibonacci(5) == 5

/**
# Compile-Time Bitwise Operations

Bitwise operations can also be performed at compile time using `#[Swag.ConstExpr]`.
In this example, we use it to check if a specific bit is set in a number.
*/
#[Swag.ConstExpr]
func isBitSet(num: s32, bit: s32) -> bool
{
    return (num & (1 << bit)) != 0
}

// This assertion checks if the 3rd bit (0-indexed) in the number 8 is set. It is,
// because 8 in binary is 1000, so this check is evaluated at compile time.
#assert isBitSet(8, 3) == true
