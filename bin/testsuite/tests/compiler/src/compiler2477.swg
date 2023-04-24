#global testerror

#[Swag.Safety("overflow", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: u32 = Swag.U32.Max
    var y: u32 = 2
    x = x * y
}

#[Swag.Safety("overflow", true)]
#[Swag.Optimize("", false)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
