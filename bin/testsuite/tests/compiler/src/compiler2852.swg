#test
{
    {
        let x  = -128's8
        let x1 = -0x80's8
        @assert(x == -128)
        @assert(x1 == -128)
    }

    {
        let x = -0x8000's16
        @assert(x == Swag.S16.Min)
    }

    {
        let x = -0x80000000's32
        @assert(x == Swag.S32.Min)
    }

    {
        let x = -0x80000000_00000000's64
        @assert(x == Swag.S64.Min)
    }
}
