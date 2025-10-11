#global testerror "Saf0027"
//#[Swag.PrintBc]
#[Swag.Optimize("", false), Swag.Safety("", true)]
#test
{
    var x = -128's8
    x = -x
    @print(x)
}
