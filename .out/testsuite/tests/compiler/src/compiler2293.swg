//#[Swag.PrintBc]
#test
{
    {
        x := 10's32
        @atomadd(&x, -1)
        @assert(x == 9)
    }

    {
        x := 10'u8
        y := 20'u8
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        x := 10's8
        y := 20's8
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        x := 10's16
        y := 20's16
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        x := 10's32
        y := 20's32
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        x := 10's64
        y := 20's64
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }
}