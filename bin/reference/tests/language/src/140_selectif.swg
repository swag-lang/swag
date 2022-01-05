#test
{
    // On a function, you can use '#selectif' to constrain the usage of the function.
    // If the #selectif expression returns false, then the function will not be considered for the call match.
    // The #selectif expression is evaluated only once, whatever the call, so it is typically used to
    // check generic parameters.
    {
        func(T) sum(x: T...)->T
            #selectif @typeof(T) == s32 or @typeof(T) == s64
        {
            total := 0'T
            visit it: x
                total += it
            return total
        }

        res1 := sum's32(1, 2)
        @assert(res1 == 3)
        res2 := sum's64(10, 20)
        @assert(res2 == 30)

        // This would generate a compile error because type is 'f32'
        //res1 := sum'f32(1, 2)
    }

    // You can use #selectif to make a kind of generic specialisation
    {
        // s32 version
        func(T) isNull(x: T)->bool
            #selectif @typeof(T) == s32
        {
            return x == 0
        }

        // f32/f64 version
        func(T) isNull(x: T)->bool
            #selectif @typeof(T) == f32 or @typeof(T) == f64
        {
            return @abs(x) < 0.01
        }

        @assert(isNull(0's32))
        @assert(isNull(0.001'f32))
    }

    // #selectif can also be followed by a block that returns a bool instead of an expression
    {
        func(T) sum(x: T...)->T
            #selectif
            {
                if @typeof(T) == s32 or @typeof(T) == s64
                    return true
                return false
            }
        {
            total := 0'T
            visit it: x
                total += it
            return total
        }
    }

    // By using @errormsg, you can trigger errors at compile time if the
    // type is incorrect for example
    {
        func(T) sum(x, y: T)->T
            #selectif
            {
                if @typeof(T) == s32 or @typeof(T) == s64
                    return true
                @errormsg("invalid type " ++ @stringof(T), #location)
                return false
            }
        {
            return x + y
        }

        // This will trigger an error
        //x := sum'f32(1, 2)
    }

    // Instead of #selectif, you can use #checkif.
    // #checkif is evaluated for each call, so it can be used to check
    // parameters, as long as they can be evaluated at compile time
    {
        func div(x, y: s32)->s32
            #checkif
            {
                // If 'y' cannot be evaluated at compile time, then we can do nothing about it
                if !@isconstexpr(y)
                    return true
                if y == 0
                    @errormsg("division by zero", #location)
                return true
            }
        {
            return x / y
        }

        // Fine
        x1 := div(1, 1)

        // Error at compile time, division by zero
        //x2 := div(1, 0)
    }

    {
        // A version of 'first' where 'x' is known at compile time
        func first(x: s32)->s32
            #checkif @isconstexpr(x)
        {
            return 555
        }

        // A version of 'first' where 'x' is *NOT* known at compile time
        func first(x: s32)->s32
            #checkif !@isconstexpr(x)
        {
            return 666
        }

        // Will call version 1 because parameter is a literal
        @assert(first(0) == 555)

        // Will call version 2 because parameter is a variable
        var a: s32
        @assert(first(a) == 666)
    }
}