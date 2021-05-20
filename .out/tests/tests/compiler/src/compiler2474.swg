#global testerror
#global testerror

#[Swag.safety("overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x = x * 2
}

#[Swag.safety("overflow", true)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
