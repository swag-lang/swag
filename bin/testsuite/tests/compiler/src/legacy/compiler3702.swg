#[Swag.Optimize(.All, false)]
#test
{
    {
        let a = 1'u8
        let b = 3'u8
        let c = a ^ b
        @assert(c == 2)
    }

    {
        let a = 1'u16
        let b = 3'u16
        let c = a ^ b
        @assert(c == 2)
    }
}
