/**
# 'Single Evaluation'
The 'where' clause in Swag applies constraints on function invocations, ensuring
they can only be called when specific conditions are met. This is especially
useful in generic functions where you want to restrict permissible types or values.

When the 'where' expression evaluates to false, the function is not considered
during the call; if no alternative overloads match, the compiler raises an error.
The 'where' expression is evaluated only once, usually during function instantiation,
making it ideal for stable constraints on generic parameters.
*/

#test
{
    // Validate the type: only 's32' or 's64' are accepted for T.
    func(T) sum(x: T...)->T
        where T == s32 or T == s64
    {
        var total = 0'T
        foreach it in x do
            total += it
        return total
    }

    // Valid calls: T is 's32' or 's64'.
    let res1 = sum's32(1, 2)
    @assert(res1 == 3)

    let res2 = sum's64(10, 20)
    @assert(res2 == 30)

    // The following would be an error: 'f32' is not accepted.
    // var res3 = sum'f32(1, 2)
}

/**
# 'Generic Specialization'
The 'where' clause supports specialized implementations of generic functions.
You can provide distinct implementations based on type or value to improve
clarity and efficiency.
*/

#test
{
    // Specialization for 's32'.
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where T == s32
    {
        return x == 0
    }

    // Specialization for 'f32' and 'f64'.
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
# 'Block-based where' Clause
The 'where' clause can be a block returning a bool, which enables more
complex compile-time checks.
*/

#test
{
    // Function accepts only T in {'s32', 's64'} via a block-based condition.
    func(T) sum(x: T...)->T
        where
        {
            if #typeof(T) == s32 or #typeof(T) == s64 do
                return true
            return false
        }
    {
        var total = 0'T
        foreach it in x do
            total += it
        return total
    }
}

/**
# 'Custom Compile-time Errors'
Using the '@compilererror' intrinsic, you can emit custom compile-time errors
when 'where' conditions fail. This guides users with clear diagnostics.
*/

#test
{
    func(T) sum(x, y: T)->T
        where
        {
            if T == s32 or T == s64 do
                return true

            @compilererror("Invalid type " ++ #stringof(T), #location(T))
            return false
        }
    {
        return x + y
    }

    // This would trigger a compile-time error: 'f32' is not valid here.
    // var x = sum'f32(1, 2)
}

/**
# 'Generic Structs' with 'where'
The 'where' clause also applies to generic structs. If the condition is
not met, an error is emitted immediately (there is no overload resolution
for structs).
*/

#test
{
    // Point constrained to floating types 'f32' or 'f64'.
    struct(T) Point
            where T == f32 or T == f64
    {
        x, y: T
    }

    // Valid instantiation with 'f32'.
    var v: Point'f32

    // Error: 's32' is not permitted by the struct constraint.
    // var v: Point's32
}

/**
# 'Multiple Evaluations' with 'verify'
With 'verify' mode, the constraint is evaluated for each call (rather than
once per instantiation). This suits conditions depending on actual arguments,
provided they are evaluable at compile time.
*/

#test
{
    {
        // Division function: ensure 'y' is not zero at compile time when possible.
        func div(x, y: s32)->s32
            verify
            {
                // If 'y' is not constexpr, allow the call; otherwise check for zero.
                if !#isconstexpr(y) do
                    return true
                if y == 0 do
                    @compilererror("Division by zero", #location(y))
                return true
            }
        {
            return x / y
        }

        // Valid division.
        var x1 = div(1, 1)

        // Compile-time error example:
        // var x2 = div(1, 0)
    }

    {
        // Overload selected when 'x' is known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            verify #isconstexpr(x)
        {
            return 555
        }

        // Overload selected when 'x' is not known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            verify !#isconstexpr(x)
        {
            return 666
        }

        // Literal: chooses the constexpr version.
        @assert(first(0) == 555)

        // Variable: chooses the non-constexpr version.
        var a: s32
        @assert(first(a) == 666)
    }
}
