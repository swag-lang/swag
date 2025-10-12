#global testerror "Err0500"

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    let x = "toto"
    let z = x[5]
}
