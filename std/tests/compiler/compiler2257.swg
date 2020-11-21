//#[swag.printbc]
#test
{
    {
        x := 0's8
        @assert(@atomadd(&x, 1) == 0)
        @assert(@atomadd(&x, 2) == 1)
    }

    {
        x := 0's16
        @assert(@atomadd(&x, 1) == 0)
        @assert(@atomadd(&x, 2) == 1)
    }

    {
        x := 0's32
        @assert(@atomadd(&x, 1) == 0)
        @assert(@atomadd(&x, 2) == 1)
    }

    {
        x := 0's64
        @assert(@atomadd(&x, 1) == 0)
        @assert(@atomadd(&x, 2) == 1)
    }
}