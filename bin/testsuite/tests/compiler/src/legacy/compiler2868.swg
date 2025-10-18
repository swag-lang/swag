#global testerror "Err0571"

//#[Swag.Optimize(.All, false)]
#[Swag.Safety(.All, .All, true)]
#test
{
    let y = Swag.S32.Min
    var x = -y
    x += 1
}
