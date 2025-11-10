/**
# '@generic' Functions
A function can be made generic by specifying type parameters after the `func` keyword.
These type parameters allow the function to operate on various types using the same implementation.
The generic type parameters are placed within parentheses after `func`. When calling the function,
the generic types are specified using `funcCall'(type1, type2, ...)`. If there is only one generic
parameter, you can omit the parentheses.
*/
#test
{
    {
        // Generic function where 'T' is the type parameter.
        func(var T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4) // Explicitly using 's32' as the generic type.
        @assert(myFunc'f32(2.0) == 4.0) // Explicitly using 'f32' as the generic type.
    }

    {
        // Declaring a generic function without 'var'.
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4) // Type 's32' is inferred as the generic type.
        @assert(myFunc'f32(2.0) == 4.0) // Type 'f32' is inferred as the generic type.
    }

    {
        // Generic function with a default type parameter.
        func(T = s32) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4) // Uses default type 's32'.
        @assert(myFunc'f32(2.0) == 4.0) // Overrides the default type with 'f32'.
    }

    {
        // Function with multiple generic parameters.
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value") // K and V inferred from arguments.
        @assert(myFunc'(s32, string)(2, "value") == "value") // K and V explicitly specified.

        @assert(myFunc(2's32, true) == true) // Type deduction for both K and V.
        @assert(myFunc'(s32, bool)(2, true) == true) // Explicit type declaration for K and V.
    }
}

/**
# Type Deduction
Generic types can often be deduced from the function's parameters,
eliminating the need to specify the type explicitly at the call site.
*/
#test
{
    func(T) myFunc(val: T) => 2 * val

    @assert(myFunc(2's32) == 4) // Type 'T' deduced as 's32'.
    @assert(myFunc(2.0'f32) == 4.0) // Type 'T' deduced as 'f32'.
}

/**
# Using Constants as Generic Parameters
In addition to types, constants can also be used as generic parameters.
*/

/**
In this example, `N` is a constant of type `s32`.
*/
#test
{
    func(const N: s32) myFunc() { @assert(N == 10) }
    myFunc'10() // Calls the function with constant value 10.
}

/**
`const` can be omitted when declaring constants, as an identifier followed by a type
is treated as a constant.
*/
#test
{
    func(N: s32) myFunc() { @assert(N == 10) }
    myFunc'10() // Equivalent to using 'const'.
}

/**
You can also assign a default value to a constant parameter.
*/
#test
{
    func(N: s32 = 10) myFunc() { @assert(N == 10) }
    myFunc() // Uses the default constant value 10.
}

/**
If you declare the constant using `const`, the type can be omitted,
and it will be deduced from the assigned value.
*/
#test
{
    func(const N = 10) myFunc() { @assert(N == 10) }
    myFunc() // Type of N deduced from literal 10.
}

/**
# Mixing Types and Constants
You can mix type parameters and constant parameters within the same generic function.
*/
#test
{
    {
        // Example where 'T' is a type and 'N' is a constant of type s32.
        func(T, N: s32) myFunc(x: T) => x * N

        alias call = myFunc'(s32, 10)
        @assert(call(2) == 20) // 's32' type and constant 10 used.
        @assert(call(100) == 1000) // Same type and constant reused.
    }

    {
        // Example using multiple constant parameters.
        func(T: s32, N: s32) myFunc() => T * N

        @assert(myFunc'(5, 10)() == 50) // Function called with two s32 constants.
    }

    {
        // Multiple type parameters with default values.
        func(T = s32, V = s32) myFunc(x: T, y: V) => x * y

        @assert(myFunc(1's32, 2'f32) == 2.0) // Mixed s32 and f32, type deduced.
        @assert(myFunc(1's32, 2's32) == 2) // Both parameters use s32.
    }
}
