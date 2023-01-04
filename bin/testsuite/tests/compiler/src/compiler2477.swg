#global testerror
#global testerror

#[Swag.Safety("over", true)]
#test
{
    var x: u32 = Swag.U32.Max
    var y: u32 = 2
    x = x * y
}

#[Swag.Safety("over", true)]
#test
{
    var x: u64 = Swag.U64.Max
    x = x * 2
}
