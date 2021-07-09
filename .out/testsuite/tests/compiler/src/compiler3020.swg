#test
{
    {
        x, y := true
        x ^= y
        @assert(x == false)
    }

    {
        x, y := false
        x ^= y
        @assert(x == false)
    }

    {
        x := false
        y := true
        x ^= y
        @assert(x == true)
    }

    {
        x := true
        y := false
        x ^= y
        @assert(x == true)
    }
}