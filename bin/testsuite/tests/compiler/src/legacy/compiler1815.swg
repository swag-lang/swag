#global testerror "Err0485"

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    let a = "toto"
    #[Swag.Safety(.All, .All, true)]
    let c = a[0 to 4]
}
