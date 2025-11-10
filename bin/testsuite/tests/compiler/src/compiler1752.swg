#global testerror "Saf0004"

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    var x = "64"
    #[Swag.Safety(.All, .All, true)]
    let y = &x[3]
}
