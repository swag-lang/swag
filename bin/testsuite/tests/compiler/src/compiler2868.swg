#global testerror "Err0294"

//#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#test
{
    let y = Swag.S32.Min
    var x = -y
    x += 1
}