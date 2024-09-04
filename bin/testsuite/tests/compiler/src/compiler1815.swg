
#global testerror "Err0491"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    let a = "toto"
    #[Swag.Safety("", true)]
    let c = a[0..4]
}