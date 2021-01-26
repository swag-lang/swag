#global testerror
#global testerror
#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: u8 = swag.MaxU8
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    var x: u16 = swag.MaxU16
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    var x: u32 = swag.MaxU32
    x *= 2
}

#[swag.safety("of", true)]
#test
{
    #[swag.safety("of", true)]
    var x: u64 = swag.MaxU64
    x *= 2
}
