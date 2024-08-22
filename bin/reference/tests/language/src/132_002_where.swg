/**
# Single Evaluation
The `where` clause in Swag can be used to apply constraints on functions, ensuring that they are only callable when specific conditions are met. This is particularly useful for generic functions, where you want to restrict the types or values that can be passed as arguments.

If the `where` expression returns `false`, the function will not be considered for the call. If no other overload matches, the compiler will raise an error. The `where` expression is evaluated **only once**, typically when the function is instantiated, making it ideal for applying constraints on generic parameters.
*/

#test
{
    // Here we validate the function only if the generic type is `s32` or `s64`.
    func(T) sum(x: T...)->T
        where T == s32 or T == s64
    {
        var total = 0'T
        visit it in x:
            total += it
        return total
    }

    // These calls are valid because `T` is `s32` or `s64`.
    let res1 = sum's32(1, 2)
    @assert(res1 == 3)
    let res2 = sum's64(10, 20)
    @assert(res2 == 30)

    // The following would generate an error because the type is `f32`, and there's no matching overload.

    // var res3 = sum'f32(1, 2)
}

/**
# Generic Specialization
You can use the `where` clause to create specialized versions of a generic function. This allows you to provide different implementations based on the type or value of the parameters.
*/

#test
{
    // Specialized version for `s32`.
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where T == s32
    {
        return x == 0
    }

    // Specialized version for `f32` and `f64`.
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where T == f32 or T == f64
    {
        return @abs(x) < 0.01
    }

    @assert(isNull(0's32))
    @assert(isNull(0.001'f32))
}

/**
# Block-based `where` Clause
Instead of a single expression, the `where` clause can be followed by a block that returns a `bool` value. This allows for more complex conditions that might involve multiple checks or custom logic.
*/

#test
{
    func(T) sum(x: T...)->T
        where
        {
            if @typeof(T) == s32 or @typeof(T) == s64:
                return true
            return false
        }
    {
        var total = 0'T
        visit it in x:
            total += it
        return total
    }
}

/**
# Custom Compile-time Errors
Using the `@compilererror` intrinsic, you can trigger custom compile-time errors if the `where` condition is not met. This allows for clear and specific error messages when the function is used incorrectly.
*/

#test
{
    func(T) sum(x, y: T)->T
        where
        {
            if T == s32 or T == s64:
                return true
            @compilererror("Invalid type " ++ @stringof(T), @location(T))
            return false
        }
    {
        return x + y
    }

    // This will trigger an error because `f32` is not a valid type.

    // var x = sum'f32(1, 2)
}

/**
# Generic Structs with `where`
The `where` clause can also be used with generic structs. If the condition is not met, an error will be generated immediately since there is no overload resolution for structs.
*/

#test
{
    struct(T) Point
        where T == f32 or T == f64
    {
        x, y: T
    }

    // Valid instantiation with `f32`.
    var v: Point'f32

    // The following would generate an error because `s32` is not allowed.

    // var v: Point's32
}

/**
# Multiple Evaluations
By using the `where(call)` mode, the `where` clause is evaluated for **each** function call, instead of just once per function instantiation. This is useful for checking conditions that depend on the actual arguments passed to the function, as long as these arguments can be evaluated at compile time.
*/

#test
{
    {
        // Example where `y` must not be zero at compile time.
        func div(x, y: s32)->s32
            where(call)
            {
                // Check if `y` is a compile-time constant.
                if !@isconstexpr(y):
                    return true
                if y == 0:
                    @compilererror("Division by zero", @location(y))
                return true
            }
        {
            return x / y
        }

        // Valid division.
        var x1 = div(1, 1)

        // The following would generate a compile-time error due to division by zero.

        // var x2 = div(1, 0)
    }

    {
        // A function with different implementations based on whether `x` is known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            where(call) @isconstexpr(x)
        {
            return 555
        }

        // Another overload for the case where `x` is not known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            where(call) !@isconstexpr(x)
        {
            return 666
        }

        // Will call the first version because `x` is a literal.
        @assert(first(0) == 555)

        // Will call the second version because `x` is a variable.
        var a: s32
        @assert(first(a) == 666)
    }
}
