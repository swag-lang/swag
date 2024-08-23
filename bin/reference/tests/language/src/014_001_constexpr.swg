/**
# Compile-Time Function Evaluation with `#[Swag.ConstExpr]`

The `#[Swag.ConstExpr]` attribute marks a function as capable of being evaluated during compile time. 
This enables the compiler to resolve the function's result at compile time, provided that all inputs 
are also known at compile time. This approach significantly optimizes the code by precomputing values 
and eliminating the need for these calculations at runtime. 

Functions marked with `#[Swag.ConstExpr]` are ideal for returning constant values or performing operations 
that are determined before runtime, thereby improving efficiency.
*/

// The function 'isThisDebug' is annotated with 'Swag.ConstExpr', indicating that 
// it can be evaluated during the compilation phase. Given that this function 
// consistently returns `true`, the result is established at compile time.
#[Swag.ConstExpr]
func isThisDebug() => true

// This conditional block demonstrates how 'isThisDebug' can be utilized in a 
// compile-time context. Since 'isThisDebug' invariably returns `true`, the 
// condition `isThisDebug() == false` evaluates to `false`, and thus the compiler 
// will exclude the code inside the block from the final compilation.
#if isThisDebug() == false
{
    #error "this should not be called!"
}

/**
# Recursive Compile-Time Evaluation

The `#[Swag.ConstExpr]` attribute can also be applied to more complex functions, 
including those that perform recursion. This allows such recursive functions to 
be entirely evaluated at compile time, effectively reducing runtime overhead 
and improving overall performance.
*/

// This function 'factorial' calculates the factorial of a given number recursively. 
// By marking it with 'Swag.ConstExpr', the factorial is computed during compilation, 
// avoiding the need for runtime computation.
#[Swag.ConstExpr]
func factorial(x: s32) -> s32
{
    // Base case: return 1 when x equals 1
    if x == 1:
        return 1
    
    // Recursive case: multiply x by the factorial of (x - 1)
    return x * factorial(x - 1)
}

// The `#assert` directive ensures that 'factorial(4)' equals 24. As 'factorial' 
// is evaluated at compile time, this assertion is verified before execution begins.
#assert factorial(4) == 24 // Evaluated at compile time

/**
# Compile-Time Constant Expressions

In this section, `#[Swag.ConstExpr]` is utilized to define a straightforward constant 
expression. The function returns a fixed value that the compiler resolves during compilation.
*/

// The 'getMagicNumber' function returns a constant value of 42. 
// Since it's a compile-time constant expression, this value is resolved at compile time.
#[Swag.ConstExpr]
func getMagicNumber() -> s32
{
    return 42
}

// The assertion checks that 'getMagicNumber()' equals 42, verified at compile time.
#assert getMagicNumber() == 42

/**
# Compile-Time Conditional Logic

This example illustrates how the function `isEven` determines if a number is even. 
By marking it with `#[Swag.ConstExpr]`, the compiler can perform this logic during 
the compilation phase.
*/

// The 'isEven' function checks whether a number is even. 
// With the 'Swag.ConstExpr' annotation, this check occurs at compile time.
#[Swag.ConstExpr]
func isEven(x: s32) -> bool
{
    return x % 2 == 0
}

// This block only compiles if the number 4 is even. Since 4 is indeed even, 
// the error is not triggered, and the code compiles successfully.
#if isEven(4) == false
{
    #error "4 should be even!"
}

/**
# Compile-Time Slice Operations

In this example, `#[Swag.ConstExpr]` is used to calculate the sum of elements within an array. 
The summation is performed during the compilation, optimizing runtime performance.
*/

// The 'arraySum' function calculates the sum of all elements in an array. 
// Since it is a compile-time function, the sum is computed during the compilation phase.
#[Swag.ConstExpr]
func arraySum(arr: const [..] s32) -> s32
{
    var sum = 0
    visit val in arr:
        sum += val
    return sum
}

// The assertion verifies that the sum of the array [1, 2, 3, 4, 5] equals 15. 
// This is checked and confirmed at compile time.
#assert arraySum([1, 2, 3, 4, 5]) == 15

/**
# Compile-Time Fibonacci Sequence

This example showcases how `#[Swag.ConstExpr]` enables a recursive function to 
compute the Fibonacci sequence at compile time, optimizing the execution.
*/

// The 'fibonacci' function calculates the nth Fibonacci number recursively. 
// The result is computed during the compilation when marked with 'Swag.ConstExpr'.
#[Swag.ConstExpr]
func fibonacci(n: s32) -> s32
{
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)
}

// The 5th Fibonacci number, which is 5, is calculated during compilation and asserted here.
#assert fibonacci(5) == 5

/**
# Compile-Time Bitwise Operations

Bitwise operations can also be evaluated at compile time using `#[Swag.ConstExpr]`. 
This example demonstrates how to check if a specific bit is set within a number.
*/

// The 'isBitSet' function checks whether a particular bit is set in a number. 
// With 'Swag.ConstExpr', this operation is performed at compile time.
#[Swag.ConstExpr]
func isBitSet(num: s32, bit: s32) -> bool
{
    return (num & (1 << bit)) != 0
}

// The assertion verifies that the 3rd bit (0-indexed) of the number 8 is set. 
// Since 8 in binary is 1000, the check is resolved during compilation.
#assert isBitSet(8, 3) == true
