//#[swag.printbc]
#test
{
    {
        x := 7's8
        @assert(@atomcmpxchg(&x, 7, 3) == 7)
        @assert(x == 3)
        @assert(@atomcmpxchg(&x, 3, 5) == 3)
        @assert(x == 5)
        @assert(@atomcmpxchg(&x, 66, 66) == 5)
        @assert(x == 5)
    }

    {
        x := 7's16
        @assert(@atomcmpxchg(&x, 7, 3) == 7)
        @assert(x == 3)
        @assert(@atomcmpxchg(&x, 3, 5) == 3)
        @assert(x == 5)
        @assert(@atomcmpxchg(&x, 66, 66) == 5)
        @assert(x == 5)
    }

    {
        x := 7's32
        @assert(@atomcmpxchg(&x, 7, 3) == 7)
        @assert(x == 3)
        @assert(@atomcmpxchg(&x, 3, 5) == 3)
        @assert(x == 5)
        @assert(@atomcmpxchg(&x, 66, 66) == 5)
        @assert(x == 5)
    }

    {
        x := 7's64
        @assert(@atomcmpxchg(&x, 7, 3) == 7)
        @assert(x == 3)
        @assert(@atomcmpxchg(&x, 3, 5) == 3)
        @assert(x == 5)
        @assert(@atomcmpxchg(&x, 66, 66) == 5)
        @assert(x == 5)
    }
}