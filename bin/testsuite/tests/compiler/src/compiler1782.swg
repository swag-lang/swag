
#global testerror "Err0500"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = "toto"
    let z = x[5]
}