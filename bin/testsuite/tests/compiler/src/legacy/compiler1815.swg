#global testerror "Err0502"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    let a = "toto"
    #[Swag.Safety("", true)]
    let c = a[0 to 4]
}
