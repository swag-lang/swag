
#global testerror "Err0133"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = "toto"
    let z = x[5]
}