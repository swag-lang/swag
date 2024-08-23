/**
# Generic Functions
A function can be made generic by specifying type parameters after the `func` keyword. These type 
parameters allow the function to operate on various types using the same implementation. The generic 
type parameters are placed within parentheses after `func`. When calling the function, the generic 
types are specified using `funcCall'(type1, type2, ...)`. If there is only one generic parameter, 
you can omit the parentheses.
*/

#test
{
    {
        // Example of a generic function where 'T' is the generic type.
        func(var T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)             // Explicitly passing 's32' as the generic type.
        @assert(myFunc'f32(2.0) == 4.0)         // Explicitly passing 'f32' as the generic type.
    }

    {
        // Declaring the generic type without 'var'.
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)             // Type 's32' is inferred as the generic type.
        @assert(myFunc'f32(2.0) == 4.0)         // Type 'f32' is inferred as the generic type.
    }

    {
        // Setting a default value for the generic type.
        func(T = s32) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)             // Uses default type 's32'.
        @assert(myFunc'f32(2.0) == 4.0)         // Overrides the default type with 'f32'.
    }

    {
        // Using multiple generic parameters.
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value")     // Both K and V are deduced from the parameters.
        @assert(myFunc'(s32, string)(2, "value") == "value") // K and V are specified explicitly.

        @assert(myFunc(2's32, true) == true)           // Type deduction used for both K and V.
        @assert(myFunc'(s32, bool)(2, true) == true)   // Explicit type declaration for K and V.
    }
}

/**
# Type Deduction
Generic types can often be deduced from the function's parameters, eliminating the need to specify 
the type explicitly at the call site.
*/

#test
{
    func(T) myFunc(val: T) => 2 * val

    @assert(myFunc(2's32) == 4)               // The type 'T' is deduced as 's32'.
    @assert(myFunc(2.0'f32) == 4.0)           // The type 'T' is deduced as 'f32'.
}

/**
# Using Constants as Generic Parameters
In addition to types, you can also specify constants as generic parameters.
*/

/**
In the example below, `N` is a constant of type `s32`.
*/

#test
{
    func(const N: s32) myFunc() = @assert(N == 10)
    myFunc'10()                                // Call the function with a constant value of 10.
}

/**
`const` can be omitted when declaring constants, as an identifier followed by a type is considered 
a constant.
*/

#test
{
    func(N: s32) myFunc() = @assert(N == 10)
    myFunc'10()                                // Function called with a constant value of 10.
}

/**
You can also assign a default value to a constant parameter.
*/

#test
{
    func(N: s32 = 10) myFunc() = @assert(N == 10)
    myFunc()                                   // Calls the function using the default value of 10.
}

/**
If you declare the constant using `const`, the type can be omitted, and it will be deduced from 
the assignment expression.
*/

#test
{
    func(const N = 10) myFunc() = @assert(N == 10)
    myFunc()                                   // Constant value is deduced as 's32'.
}

/**
# Mixing Types and Constants
You can mix types and constants in generic parameters.
*/

#test
{
    {
        // Example where `T` is a type and `N` is a constant of type `s32`.
        func(T, N: s32) myFunc(x: T) => x * N

        namealias call = myFunc'(s32, 10)
        @assert(call(2) == 20)                 // Function call with 's32' type and 10 constant.
        @assert(call(100) == 1000)             // Another call with the same type and constant.
    }

    {
        // Declaring multiple constants requires specifying the type for each.
        func(T: s32, N: s32) myFunc() => T * N

        @assert(myFunc'(5, 10)() == 50)        // Function call with two 's32' constants.
    }

    {
        // Declaring multiple types with default values.
        func(T = s32, V = s32) myFunc(x: T, y: V) => x * y

        @assert(myFunc(1's32, 2'f32) == 2.0)   // Calls with 's32' and 'f32', type deduced.
        @assert(myFunc(1's32, 2's32) == 2)     // Calls with 's32' for both parameters.
    }
}
