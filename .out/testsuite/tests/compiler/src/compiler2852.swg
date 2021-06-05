#test
{
    {
        x := -128's8
        x1 := -0x80's8
        @assert(x == -128)
        @assert(x1 == -128)
    }

    {
        x := -0x8000's16
        @assert(x == Swag.S16.Min)
    }

    {
        x := -0x80000000's32
        @assert(x == Swag.S32.Min)
    }

    {
        x := -0x80000000_00000000's64
        @assert(x == Swag.S64.Min)
    }
}