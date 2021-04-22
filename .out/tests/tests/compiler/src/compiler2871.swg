//#[swag.printbc]
#test
{
    {
        cpt := 0
        l := 0
        loop x: -5..5
        {
            cpt += x
            l += 1
        }

        @assert(cpt == 0)
        @assert(l == 11)
    }

    {
        l := 0
        loop i: 1..3
        {
            l += i
        }
        @assert(l == 6)
    }

    {
        l := 0
        loop i: 1..1
        {
            l += i
        }
        @assert(l == 1)
    }

    {
        l := 0
        c := 0
        loop i: 4..0
        {
            l += i
            c += 1
        }

        @assert(l == 10)
        @assert(c == 5)
    }
}