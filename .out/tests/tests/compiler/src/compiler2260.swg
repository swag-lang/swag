//#[swag.printbc]
#test
{
    {
        x := 7'u8
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7'u16
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7'u32
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }

    {
        x := 7'u64
        @assert(@atomxor(&x, 3) == 7)
        @assert(@atomxor(&x, 4) == 4)
        @assert(x == 0)
    }
}