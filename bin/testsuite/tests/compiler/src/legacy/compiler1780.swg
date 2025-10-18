#global testerror "Err0410"

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, true)]
#test
{
    var x = 10
    x = x % 0
}
