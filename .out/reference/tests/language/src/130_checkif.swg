#test
{
    // On a function, you can use '#checkif' to verify parameters
    // The compiler will generate an error if the #checkif returns false
    {
        func(T) sum(x: T...)->T
            #checkif @typeof(T) == s32 or @typeof(T) == s64
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

    // You can check some parameters too, as long as it can be evaluated at compile time
    {
        func whatever(x: ...)
            #checkif
            {
                if @countof(x) == 4
                    return true
                return false
            }
        {
        }

        // Fine, 4 parameters
        whatever(1, 2, 3, 4)

        // Error, only 3
        //whatever(1, 2, 3)
    }
}