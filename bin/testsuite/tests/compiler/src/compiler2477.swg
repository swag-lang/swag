#global testerror "San0009"
#[Swag.Safety("overflow", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u32 = Swag.U32.Max
    let y: u32 = 2
    x = x * y
}
