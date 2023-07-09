//#global skip

#[Swag.Optim("", false)]
#test
{
    {
        var v = 126's8
        var v1: s8 = v + 1
        @assert(v1 == 127)
    }

    {
        var v = 126's16
        var v1: s16 = v + 1
        @assert(v1 == 127)
    }

    {
        var v = 126'u8
        var v1: u8 = v + 1
        @assert(v1 == 127)
    }

    {
        var v = 126'u16
        var v1: u16 = v + 1
        @assert(v1 == 127)
    }
}