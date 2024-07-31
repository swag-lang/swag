
#global testerror "Err0060"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#test
{
    var x = 10
    x = x % 0
}