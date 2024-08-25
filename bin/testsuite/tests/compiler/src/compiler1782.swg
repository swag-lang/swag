
#global testerror "Err0512"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = "toto"
    let z = x[5]
}