//#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("overflow", false)]
#test
{
    mant := 0x12345678_12345678
    exp := -385's64
    if exp <= 0 and mant == (mant >> cast(u32) (-exp)) << cast(u32)(-exp)
    {
        @assert(false)
    }
}
