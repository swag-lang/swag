#global testerror "Err0651"

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, true)]
#test
{
    var x = 10
    x = x % 0
}
