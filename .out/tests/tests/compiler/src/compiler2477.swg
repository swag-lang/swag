#global testerror
#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: u32 = swag.U32.Max
    var y: u32 = 2
    x = x * y
}

#[swag.safety("overflow", true)]
#test
{
    var x: u64 = swag.U64.Max
    x = x * 2
}
