/**
# 'Compile-Time Function Evaluation with #[Swag.ConstExpr]'

The '#[Swag.ConstExpr]' attribute marks a function as evaluable during
compile time. If all inputs are known at compile time, the compiler can
resolve the function result immediately, avoiding runtime computation.

This precomputation improves efficiency and reduces runtime overhead,
making such functions ideal for constant logic or static initialization.
*/

// Simple example: always returns true, resolved at compile time.
#[Swag.ConstExpr]
func isThisDebug() => true

// Compile-time conditional block.
// Since 'isThisDebug()' is true, the code inside will never compile.
#if isThisDebug() == false
{
    #error("this should not be called!")
}

/**
# 'Recursive Compile-Time Evaluation'

Recursive functions can also be marked 'ConstExpr'. Swag evaluates
them entirely at compile time, eliminating runtime cost.
*/
#[Swag.ConstExpr]
func factorial(x: s32)->s32
{
    if x == 1 do
        return 1
    return x * factorial(x - 1)
}

#assert(factorial(4) == 24)

/**
# 'Compile-Time Constant Expressions'

ConstExpr functions can return fixed values that the compiler resolves
immediately during compilation.
*/
#[Swag.ConstExpr]
func getMagicNumber()->s32
{
    return 42
}

#assert(getMagicNumber() == 42)

/**
# 'Compile-Time Conditional Logic'

Logic such as even/odd checks can be done at compile time.
*/
#[Swag.ConstExpr]
func isEven(x: s32)->bool
{
    return x % 2 == 0
}

#if isEven(4) == false
{
    #error("4 should be even!")
}

/**
# 'Compile-Time Slice Operations'

ConstExpr functions can iterate and compute over constant arrays.
*/
#[Swag.ConstExpr]
func arraySum(arr: const [..] s32)->s32
{
    var sum = 0
    foreach val in arr do
        sum += val
    return sum
}

#assert(arraySum([1, 2, 3, 4, 5]) == 15)

/**
# 'Compile-Time Fibonacci Sequence'

Recursive computation of Fibonacci numbers at compile time.
*/
#[Swag.ConstExpr]
func fibonacci(n: s32)->s32
{
    if n <= 1 do
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)
}

#assert(fibonacci(5) == 5)

/**
# 'Compile-Time Bitwise Operations'

Bitwise operations can be resolved at compile time too.
*/
#[Swag.ConstExpr]
func isBitSet(num: s32, bit: s32)->bool
{
    return (num & (1 << bit)) != 0
}

#assert(isBitSet(8, 3) == true)
