
#global testerror "Err0566"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = "toto"
    let z = x[5]
}