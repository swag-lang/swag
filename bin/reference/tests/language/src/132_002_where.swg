/**
# Single evaluation
On a function, you can use `where` to check if the usage of the function is correct.

If the `where` expression returns false, then the function will not be considered for the call. If there's no other overload to match, then the compiler will raise an error.

The `where` expression is evaluated **only once**, whatever the call, so it is typically used to apply a **constraint** on generic parameters.
*/

#test
{
    // Here we validate the function only if the generic type is `s32` or `s64`.
    func(T) sum(x: T...)->T
        where T == s32 or T == s64
    {
        var total = 0'T
        visit it: x do
            total += it
        return total
    }

    // This is ok.
    let res1 = sum's32(1, 2)
    @assert(res1 == 3)
    let res2 = sum's64(10, 20)
    @assert(res2 == 30)

    // But the following would generate an error because the type is `f32`.
    // So there's no match possible for that type.

    //var res1 = sum'f32(1, 2)
}

/**
You can use `where` to make a kind of a generic specialisation.
*/
#test
{
    // s32 version
    #[Swag.Overload]
    func(T) isNull(x: T)->bool
        where T == s32
    {
        return x == 0
    }

    // f32/f64 version
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
Instead of a single expression, `where` can be followed by a block that needs to return a `bool` value.
*/
#test
{
    func(T) sum(x: T...)->T
        where
        {
            if @typeof(T) == s32 or @typeof(T) == s64 do
                return true
            return false
        }
    {
        var total = 0'T
        visit it: x do
            total += it
        return total
    }
}

/**
By using `@compilererror`, you can then trigger your own errors at compile time if the type is incorrect.
*/
#test
{
    func(T) sum(x, y: T)->T
        where
        {
            if T == s32 or T == s64 do
                return true
            @compilererror("invalid type " ++ @stringof(T), @location(T))
            return false
        }
    {
        return x + y
    }

    // This will trigger an error

    // var x = sum'f32(1, 2)
}

/**
`where` can also be used on a generic struct. Unlike functions, if the expression failed, then you will have an error right away because there's no overload in the case of structures.
*/
#test
{
    struct(T) Point
        where T == f32 or T == f64
    {
        x, y: T
    }

    // Fine.
    var v: Point'f32

    // Error.
    // var v: Point's32
}

/**
# Multiple evaluations
You can add the specific mode `call` just after `where`. 
The expression will then not only be called once per instance, but will be evaluated for **each** call. So it can be used to check call parameters, as long as they can be **evaluated at compile time**.
*/
#test
{
    {
        func div(x, y: s32)->s32
            where(call)
            {
                // Here we use '@isconstexpr'.
                // If 'y' cannot be evaluated at compile time, then we can do nothing about it.
                if !@isconstexpr(y) do
                    return true
                if y == 0 do
                    @compilererror("division by zero", @location(y))
                return true
            }
        {
            return x / y
        }

        // Fine
        var x1 = div(1, 1)

        // Error at compile time, division by zero.

        // var x2 = div(1, 0)
    }

    {
        // A version of 'first' where 'x' is known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            where(call) @isconstexpr(x)
        {
            return 555
        }

        // A version of 'first' where 'x' is **not** known at compile time.
        #[Swag.Overload]
        func first(x: s32)->s32
            where(call) !@isconstexpr(x)
        {
            return 666
        }

        // Will call version 1 because parameter is a literal.
        @assert(first(0) == 555)

        // Will call version 2 because parameter is a variable.
        var a: s32
        @assert(first(a) == 666)
    }
}
