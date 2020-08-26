#test
{
    {
        a := 0x3's8
        a ^= 1
        @assert(a == 2)
    }

    {
        a := 0x3's8
        a |= 4
        @assert(a == 7)
    }

    {
        a := 0x3's8
        a &= 2
        @assert(a == 2)
    }

    {
        a := 0x3's8
        a /= 2
        @assert(a == 1)
    }

    {
        a := 0x3's64
        a %= 2
        @assert(a == 1)
    }

    {
        a := 0x3'u8
        a %= 2
        @assert(a == 1)
    }

    {
        a := 0x3's8
        a -= 2
        @assert(a == 1)
    }

    {
        a := 0x3's8
        a += 2
        @assert(a == 5)
    }
}