#test
{
    // `for` accepts a *start statement*, an *expression to test*, and an *ending statement*. This is in fact the same as the C/C++ `for`.
    {
        var cpt = 0
        for var i = 0; i < 10; i += 1
            cpt += 1
        @assert(cpt == 10)

        // ';' can be replaced by a newline (like always)
        for var i = 0
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 20)

        // Instead of one single expression, you can use a statement
        for { var i = 0; cpt = 0; }
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 10)

        for { var i = 0; cpt = 0; }
            i < 10
            { i += 2; i -= 1; }
        {
            cpt += 1
        }
        @assert(cpt == 10)
    }

    // Like `loop`, `visit` and `while`, you have access to `@index`, the **current loop index**.
    {
        var cpt = 0'u64
        for var i: u32 = 10; i < 15; i += 1
            cpt += @index
        @assert(cpt == 0+1+2+3+4)

        var cpt1 = 0'u64
        for var i = 10; i < 15; i += 1
            cpt1 += @index
        @assert(cpt1 == 0+1+2+3+4)
    }
}