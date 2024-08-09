
#global testerror "Err0057"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#test
{
    var x = 10
    x = x % 0
}