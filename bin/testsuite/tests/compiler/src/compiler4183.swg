#global testerror "San0009"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
#[Swag.Safety("overflow", true)]
#test
{
    var x = 254'u8
    let y = 1'u8
    x += y
    x += y
}