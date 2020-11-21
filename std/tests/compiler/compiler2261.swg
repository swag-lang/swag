//#[swag.printbc]
#test
{
    {
        x := 7's8
        @assert(@atomcmpxchg(&x, 3, 7) == 7)
        @assert(@atomcmpxchg(&x, 5, 3) == 3)
        @assert(@atomcmpxchg(&x, 10, 3) == 5)
        @assert(x == 5)
    }

    {
        x := 7's16
        @assert(@atomcmpxchg(&x, 3, 7) == 7)
        @assert(@atomcmpxchg(&x, 5, 3) == 3)
        @assert(@atomcmpxchg(&x, 10, 3) == 5)
        @assert(x == 5)
    }

    {
        x := 7's32
        @assert(@atomcmpxchg(&x, 3, 7) == 7)
        @assert(@atomcmpxchg(&x, 5, 3) == 3)
        @assert(@atomcmpxchg(&x, 10, 3) == 5)
        @assert(x == 5)
    }

    {
        x := 7's64
        @assert(@atomcmpxchg(&x, 3, 7) == 7)
        @assert(@atomcmpxchg(&x, 5, 3) == 3)
        @assert(@atomcmpxchg(&x, 10, 3) == 5)
        @assert(x == 5)
    }
}