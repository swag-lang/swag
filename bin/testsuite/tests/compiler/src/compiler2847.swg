//#[Swag.Safety(.All, .All, true)]
//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.CanOverflow(true)]
#test
{
    let mant = 0x12345678_12345678
    let exp  = -385's64
    if exp <= 0 and mant == (mant >> cast(u32) -exp) << cast(u32) -exp
    {
        @assert(false)
    }
}
