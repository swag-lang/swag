//#[Swag.printbc]
#test
{
    {
        x := 0'u8
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0'u16
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0'u32
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }

    {
        x := 0'u64
        @assert(@atomor(&x, 1) == 0)
        @assert(@atomor(&x, 2) == 1)
        @assert(x == 3)
    }
}