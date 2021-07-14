//#[Swag.safety("", true)]
//#[Swag.PrintBc]
#[Swag.optim("", false)]
#[Swag.safety("overflow", false)]
#test
{
    mant := 0x12345678_12345678
    exp := -385'int
    if exp <= 0 and mant == (mant >> cast(u32) (-exp)) << cast(u32)(-exp)
    {
        @assert(false)
    }
}
