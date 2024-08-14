
#global testerror "Err0503"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    let a = "toto"
    #[Swag.Safety("", true)]
    let c = a[0..4]
}