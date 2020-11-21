//#[swag.printbc]
#test
{
    {
        x := 0's8
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0's16
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0's32
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0's64
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }
}