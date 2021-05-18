#global testerror
#global testerror

#[swag.safety("overflow", true)]
#test
{
    var x: u32 = swag.U32.Max
    x = x * 2
}

#[swag.safety("overflow", true)]
#test
{
    var x: u64 = swag.U64.Max
    x = x * 2
}
