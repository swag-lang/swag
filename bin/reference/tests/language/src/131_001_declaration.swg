/**
# Generic Functions
A function can be made generic by specifying type parameters after the `func` keyword. These type parameters allow the function to operate on different types while using the same code.

The generic type parameters are specified within parentheses after `func`, and when calling the function, the generic types are specified using `funcCall'(type1, type2, ...)`. If there is only one generic parameter, you can omit the parentheses.
*/

#test
{
    {
        // Example of a generic function where 'T' is the generic type.
        func(var T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)  // Explicitly passing 's32' as the generic type.
        @assert(myFunc'f32(2.0) == 4.0) // Explicitly passing 'f32' as the generic type.
    }

    {
        // Declaring the generic type without 'var'.
        func(T) myFunc(val: T) => 2 * val

        @assert(myFunc's32(2) == 4)
        @assert(myFunc'f32(2.0) == 4.0)
    }

    {
        // Setting a default value for the generic type.
        func(T = s32) myFunc(val: T) => 2 * val

        @assert(myFunc(2's32) == 4)  // Uses default type 's32'.
        @assert(myFunc'f32(2.0) == 4.0)  // Overrides the default type with 'f32'.
    }

    {
        // Using multiple generic parameters.
        func(K, V) myFunc(key: K, value: V) => value

        @assert(myFunc(2's32, "value") == "value") // Both K and V are deduced from the parameters.
        @assert(myFunc'(s32, string)(2, "value") == "value") // K and V are specified explicitly.

        @assert(myFunc(2's32, true) == true)
        @assert(myFunc'(s32, bool)(2, true) == true)
    }
}

/**
# Type Deduction
Generic types can often be deduced from the function's parameters, eliminating the need to specify the type explicitly at the call site.
*/

#test
{
    func(T) myFunc(val: T) => 2 * val

    @assert(myFunc(2's32) == 4)  // The type 'T' is deduced as 's32'.
    @assert(myFunc(2.0'f32) == 4.0)  // The type 'T' is deduced as 'f32'.
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
    myFunc'10()  // Call the function with a constant value of 10.
}

/**
`const` can be omitted when declaring constants, as an identifier followed by a type is considered a constant.
*/

#test
{
    func(N: s32) myFunc() = @assert(N == 10)
    myFunc'10()
}

/**
You can also assign a default value to a constant parameter.
*/

#test
{
    func(N: s32 = 10) myFunc() = @assert(N == 10)
    myFunc()  // Calls the function using the default value of 10.
}

/**
If you declare the constant using `const`, the type can be omitted, and it will be deduced from the assignment expression.
*/

#test
{
    func(const N = 10) myFunc() = @assert(N == 10)
    myFunc()
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
        @assert(call(2) == 20)
        @assert(call(100) == 1000)
    }

    {
        // Declaring multiple constants requires specifying the type for each.
        func(T: s32, N: s32) myFunc() => T * N

        @assert(myFunc'(5, 10)() == 50)
    }

    {
        // Declaring multiple types with default values.
        func(T = s32, V = s32) myFunc(x: T, y: V) => x * y

        @assert(myFunc(1's32, 2'f32) == 2.0)
        @assert(myFunc(1's32, 2's32) == 2)
    }
}

/**
# Generic Structs
Structs in Swag can also be made generic, allowing them to operate with different types and constants.
*/

#test
{
    {
        // Example of a generic struct where `T` is the generic type.
        struct(T) Struct
        {
            val: T
        }

        var x: Struct's32
        @assert(@typeof(x.val) == s32)  // The type of 'val' is deduced as 's32'.
        var x1: Struct'f32
        @assert(@typeof(x1.val) == f32)  // The type of 'val' is deduced as 'f32'.
    }

    {
        // A more complex example with both a type and a constant as generic parameters.
        struct(T, N: s32) Struct
        {
            val: [N] T  // An array of 'N' elements of type 'T'.
        }

        var x: Struct'(bool, 10)
        @assert(@typeof(x.val) == #type [10] bool)  // The type is an array of 10 booleans.
    }
}
