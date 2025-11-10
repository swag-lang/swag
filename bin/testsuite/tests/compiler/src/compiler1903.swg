#test
{
    {
        var a = 0x3's8
        a ^= 1
        @assert(a == 2)
    }

    {
        var a = 0x3's8
        a |= 4
        @assert(a == 7)
    }

    {
        var a = 0x3's8
        a &= 2
        @assert(a == 2)
    }

    {
        var a = 0x3's8
        a /= 2
        @assert(a == 1)
    }

    {
        var a = 0x3's64
        a %= 2
        @assert(a == 1)
    }

    {
        var a = 0x3'u8
        a %= 2
        @assert(a == 1)
    }

    {
        var a = 0x3's8
        a -= 2
        @assert(a == 1)
    }

    {
        var a = 0x3's8
        a += 2
        @assert(a == 5)
    }
}
