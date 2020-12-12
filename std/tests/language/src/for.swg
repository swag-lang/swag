#test
{
    // 'for' accepts a start statement, an expression to test, and an ending statement
    {
        cpt := 0
        for i := 0; i < 10; i += 1
            cpt += 1
        @assert(cpt == 10)

        for i := 0
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 20)

        for { i := 0; cpt = 0; }
            i < 10
            i += 1
        {
            cpt += 1
        }
        @assert(cpt == 10)

        for { i := 0; cpt = 0; }
            i < 10
            { i += 2; i -= 1; }
        {
            cpt += 1
        }
        @assert(cpt == 10)
    }

    // Like 'loop' and 'while', you have access to '@index', the current loop index as an 'u32'
    {
        cpt := 0'uint
        for var i: u32 = 10; i < 15; i += 1;
        {
            cpt += @index
        }
        @assert(cpt == 0+1+2+3+4)
    }
}