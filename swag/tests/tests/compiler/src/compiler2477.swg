#global testerror
#global testerror

#[swag.safety("of", true)]
#test
{
    var x: u32 = swag.MaxU32
    var y: u32 = 2
    x = x * y
}

#[swag.safety("of", true)]
#test
{
    var x: u64 = swag.MaxU64
    x = x * 2
}
