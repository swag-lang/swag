//#[Swag.PrintBc]
#test
{
    {
        var x = 0xFF'u8
        @assert(@atomand(&x, 3) == 0xFF)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        var x = 0xFF'u16
        @assert(@atomand(&x, 3) == 0xFF)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        var x = 0xFF'u32
        @assert(@atomand(&x, 3) == 0xFF)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        var x = 0xFF'u64
        @assert(@atomand(&x, 3) == 0xFF)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }
}
