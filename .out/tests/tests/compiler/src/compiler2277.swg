//#[swag.printbc]
#test
{
    {
        x := 0'u8
        y := x orelse 1
        @assert(y == 1)
    }

    {
        x := 0'u16
        y := x orelse 1
        @assert(y == 1)
    }

    {
        x := 0'u32
        y := x orelse 1
        @assert(y == 1)
    }

    {
        x := 0'rune
        y := x orelse 1
        @assert(y == 1)
    }
}