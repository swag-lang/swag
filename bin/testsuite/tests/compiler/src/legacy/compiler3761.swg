//#global skip

#[Swag.Optimize(.All, false)]
#test
{
    {
        let v   = 5's8
        let v1: s8 = v * 2 + 1
        @assert(v1 == 11)
    }

    {
        let v   = 5's16
        let v1: s16 = v * 2 + 1
        @assert(v1 == 11)
    }

    {
        let v   = 5'u8
        let v1: u8 = v * 2 + 1
        @assert(v1 == 11)
    }

    {
        let v   = 5'u16
        let v1: u16 = v * 2 + 1
        @assert(v1 == 11)
    }
}
