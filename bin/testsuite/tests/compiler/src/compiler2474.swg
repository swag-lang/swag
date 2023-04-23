#global testerror
#global testerror

#[Swag.Safety("overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x = x * 2
}

#[Swag.Safety("overflow", true)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
