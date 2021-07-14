#global testerror
#global testerror
#global testerror
#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: u8 = Swag.U8.Max
    x *= 2
}

#[Swag.Safety("overflow", true)]
#test
{
    var x: u16 = Swag.U16.Max
    x *= 2
}

#[Swag.Safety("overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x *= 2
}

#[Swag.Safety("overflow", true)]
#test
{
    #[Swag.Safety("overflow", true)]
    var x: u64 = Swag.U64.Max
    x *= 2
}
