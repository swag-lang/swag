//#global skip

#[Swag.Optimize(.All, false)]
#test
{
    {
        let v   = 126's8
        let v1: s8 = v + 1
        @assert(v1 == 127)
    }

    {
        let v   = 126's16
        let v1: s16 = v + 1
        @assert(v1 == 127)
    }

    {
        let v   = 126'u8
        let v1: u8 = v + 1
        @assert(v1 == 127)
    }

    {
        let v   = 126'u16
        let v1: u16 = v + 1
        @assert(v1 == 127)
    }
}
