#global testerror "Saf0027"
//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    var x = -128's8
    x = -x
    @print(x)
}
