//#[swag.printbc]
#test
{
    {
        x := 7's8
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7's16
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7's32
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7's64
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }
}