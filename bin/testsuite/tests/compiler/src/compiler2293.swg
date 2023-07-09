//#[Swag.PrintBc]
func toto()
{
    {
        var x = 10's32
        @atomadd(&x, -1)
        @assert(x == 9)
    }

    {
        var x = 10'u8
        var y = 20'u8
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        var x = 10's8
        var y = 20's8
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        var x = 10's16
        var y = 20's16
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        var x = 10's32
        var y = 20's32
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }

    {
        var x = 10's64
        var y = 20's64
        @assert(@atomxchg(&x, y) == 10)
        @assert(x == 20)
    }
}

#test
{
    toto()
}