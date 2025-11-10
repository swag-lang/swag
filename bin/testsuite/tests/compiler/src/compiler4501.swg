#test
{
    {
        let x = -1's8
        @assert(x << 8 == 0)
    }

    {
        let x = 1's8
        @assert(x << 8 == 0)
    }

    {
        let x = 1's8
        @assert(x << 7 == -128)
    }
}
