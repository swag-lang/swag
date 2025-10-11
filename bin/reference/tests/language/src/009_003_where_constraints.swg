/**
# Single Evaluation
The `where` clause in Swag is a powerful tool for applying constraints on function invocations, ensuring that they can only be called when specific conditions are met. This feature is particularly useful in generic functions where you want to restrict the permissible types or values passed as arguments.

When the `where` expression evaluates to `false`, the function will not be considered during the call. If no alternative overloads match, the compiler will raise an error. Notably, the `where` expression is evaluated **only once**, usually during the function's instantiation. This makes it ideal for applying constraints on generic parameters in a consistent and predictable manner.
*/

#test
{
    // This function validates the type and executes only if the generic type is `s32` or `s64`.
    func(T) sum(x: T...)->T
        where T == s32 or T == s64 // Restricting `T` to `s32` or `s64`
    {
        var total = 0'T
        foreach it in x do
            total += it // Accumulate the values
        return total // Return the sum
    }

    // These calls are valid since `T` is `s32` or `s64`.
    let res1 = sum's32(1, 2) // Sum with `s32`
    @assert(res1 == 3) // Assert that result is 3
    let res2 = sum's64(10, 20) // Sum with `s64`
    @assert(res2 == 30) // Assert that result is 30

    // The following would generate an error since `f32` is not a matching type.

    // var res3 = sum'f32(1, 2)
}

/**
# Generic Specialization
The `where` clause also facilitates the creation of specialized versions of generic functions. This feature allows you to provide distinct implementations based on the type or value of the parameters, enhancing flexibility and efficiency.
*/

#test
{
    // Specialized implementation for `s32`.
    #[Swag.Overload]
    func(T) isNull(x: T)->bool // Specializing for `s32`
        where T == s32
    {
        return x == 0 // Return true if `x` is zero
    }

    // Specialized implementation for `f32` and `f64`.
    #[Swag.Overload]
    func(T) isNull(x: T)->bool // Specializing for `f32` or `f64`
        where T == f32 or T == f64
    {
        return @abs(x) < 0.01 // Return true if `x` is close to zero
    }

    @assert(isNull(0's32)) // Assert true for `s32` zero
    @assert(isNull(0.001'f32)) // Assert true for `f32` close to zero
}

/**
# Block-based `where` Clause
The `where` clause can also take the form of a block returning a `bool` value. This allows for more complex conditional logic that may involve multiple checks, making it suitable for advanced validation scenarios.
*/

#test
{
    func(T) sum(x: T...)->T // Function with a block-based `where`
        where
        {
            if #typeof(T) == s32 or #typeof(T) == s64 do  // Check if `T` is `s32` or `s64`
                return true // Return true if valid
            return false // Return false otherwise
        }
    {
        var total = 0'T // Initialize total with type `T`
        foreach it in x do
            total += it // Accumulate the values
        return total // Return the sum
    }
}

/**
# Custom Compile-time Errors
Using the `@compilererror` intrinsic, you can trigger custom compile-time errors when the `where` condition is not met. This provides a mechanism for generating clear and specific error messages, guiding users when a function is used incorrectly.
*/

#test
{
    func(T) sum(x, y: T)->T // Function with custom compile-time error
        where
        {
            if T == s32 or T == s64 do  // Check if `T` is `s32` or `s64`
                return true // Return true if valid

            @compilererror("Invalid type " ++ #stringof(T), #location(T))
            return false // Return false after error
        }
    {
        return x + y // Return the sum of `x` and `y`
    }

    // This will trigger a compile-time error as `f32` is not a valid type.

    // var x = sum'f32(1, 2)
}

/**
# Generic Structs with `where`
The `where` clause can also be applied to generic structs. If the condition is not met, an error will be generated immediately since there is no overload resolution for structs, providing a direct mechanism for enforcing constraints on struct instantiation.
*/

#test
{
    struct(T) Point
        // Struct with type constraints
            where T == f32 or T == f64
    {
        x, y: T // Coordinates with type `T`
    }

    // Valid instantiation with `f32`.
    var v: Point'f32 // Create a Point with `f32`

    // The following would generate an error since `s32` is not allowed.

    // var v: Point's32
}

/**
# Multiple Evaluations with 'verify'
By utilizing the `verify` mode, the constraint is evaluated for **each** function call, rather than just once per function instantiation. This is particularly useful for conditions that depend on the actual arguments passed to the function, as long as these arguments can be evaluated at compile time.
*/

#test
{
    {
        // Function to ensure that `y` is not zero at compile time.
        func div(x, y: s32)->s32 // Division function with compile-time check
            verify
            {
                // Verify if `y` is a compile-time constant.
                if !#isconstexpr(y) do
                    return true // Allow if `y` is not constant
                if y == 0 do
                    @compilererror("Division by zero", #location(y))
                return true // Allow otherwise
            }
        {
            return x / y
        }

        // Valid division.
        var x1 = div(1, 1) // Division with non-zero `y`

        // The following would generate a compile-time error due to division by zero.

        // var x2 = div(1, 0)
    }

    {
        // Function with different implementations based on whether `x` is known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32 // Overload for constexpr `x`
            verify #isconstexpr(x)
        {
            return 555 // Return 555 if `x` is constexpr
        }

        // Overload for the case where `x` is not known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32 // Overload for non-constexpr `x`
            verify !#isconstexpr(x)
        {
            return 666 // Return 666 if `x` is not constexpr
        }

        // Will call the first version because `x` is a literal.
        @assert(first(0) == 555)

        // Will call the second version because `x` is a variable.
        var a: s32
        @assert(first(a) == 666)
    }
}
