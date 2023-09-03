#global testerror
#[Swag.Safety("overflow", true)]
#test
{
    var x: u32 = Swag.U32.Max
    x *= 2
}
