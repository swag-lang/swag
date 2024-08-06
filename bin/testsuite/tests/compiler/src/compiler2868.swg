#global testerror "Err0356"

//#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let y = Swag.S32.Min
    var x = -y
    x += 1
}