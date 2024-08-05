
#global testerror "Err0608"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    let a = "toto"
    #[Swag.Safety("", true)]
    let c = a[0..4]
}