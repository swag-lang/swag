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
}