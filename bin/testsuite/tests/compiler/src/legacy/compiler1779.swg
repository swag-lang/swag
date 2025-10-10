#global testerror "Saf0016"

//#[Swag.PrintBc]
#[Swag.Sanity(true)]
#test
{
    var x = 10
    x /= 0
}