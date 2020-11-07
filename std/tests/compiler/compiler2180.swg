//#[swag.printbc]
#test
{
    {
        x := 6's8
        y := 2's8
        x /= y
        @assert(x == 3)
    }

    {
        x := 6's16
        y := 2's16
        x /= y
        @assert(x == 3)
    }

    {
        x := 6's32
        y := 2's32
        x /= y
        @assert(x == 3)
    }

    {
        x := 6's64
        y := 2's64
        x /= y
        @assert(x == 3)
    }
}