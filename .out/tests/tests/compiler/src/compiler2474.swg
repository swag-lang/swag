#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: u32 = swag.MaxU32
    x = x * 2
}

#[swag.safety("of", true)]
#test
{
    var x: u64 = swag.MaxU64
    x = x * 2
}
