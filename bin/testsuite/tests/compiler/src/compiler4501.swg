#test
{
    {
        x := -1's8
        @assert(x << 8 == 0)
    }

    {
        x := 1's8
        @assert(x << 8 == 0)
    }

    {
        x := 1's8
        @assert(x << 7 == -128)
    }
}