#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: u8 = swag.U8.Max
    x *= 2
}

#[swag.safety("overflow", true)]
#test
{
    var x: u16 = swag.U16.Max
    x *= 2
}

#[swag.safety("overflow", true)]
#test
{
    var x: u32 = swag.U32.Max
    x *= 2
}

#[swag.safety("overflow", true)]
#test
{
    #[swag.safety("overflow", true)]
    var x: u64 = swag.U64.Max
    x *= 2
}
