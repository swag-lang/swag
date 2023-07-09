//#global skip

#[Swag.Optim("", false)]
#test
{
    {
        var v = 5's8
        var v1: s8 = v*2 + 1
        @assert(v1 == 11)
    }

    {
        var v = 5's16
        var v1: s16 = v*2 + 1
        @assert(v1 == 11)
    }

    {
        var v = 5'u8
        var v1: u8 = v*2 + 1
        @assert(v1 == 11)
    }

    {
        var v = 5'u16
        var v1: u16 = v*2 + 1
        @assert(v1 == 11)
    }
}