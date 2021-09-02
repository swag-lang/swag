#test
{
    {
        x, y := true
        z := x ^ y
        @assert(z == false)
    }

    {
        x := true
        y := false
        z := x ^ y
        @assert(z == true)
    }

    {
        y := true
        x := false
        z := x ^ y
        @assert(z == true)
    }

    {
        x, y := false
        z := x ^ y
        @assert(z == false)
    }
}