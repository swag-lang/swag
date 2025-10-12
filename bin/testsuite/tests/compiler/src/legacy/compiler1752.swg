#global testerror "Saf0004"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    var x = "64"
    #[Swag.Safety("", "", true)]
    let y = &x[3]
}
