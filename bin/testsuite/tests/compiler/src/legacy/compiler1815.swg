#global testerror "Err0502"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    let a = "toto"
    #[Swag.Safety(.All, .All, true)]
    let c = a[0 to 4]
}
