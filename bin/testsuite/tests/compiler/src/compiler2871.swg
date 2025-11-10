//#[Swag.PrintBc]
#test
{
    {
        var cpt = 0
        var l   = 0
        for x in -5 to 5
        {
            cpt += x
            l += 1
        }

        @assert(cpt == 0)
        @assert(l == 11)
    }

    {
        var l = 0
        for i in 1 to 3
        {
            l += i
        }
        @assert(l == 6)
    }

    {
        var l = 0
        for i in 1 to 1
        {
            l += i
        }
        @assert(l == 1)
    }

    {
        var l = 0
        var c = 0
        for #reverse i in 0 to 4
        {
            l += i
            c += 1
        }

        @assert(l == 10)
        @assert(c == 5)
    }
}
