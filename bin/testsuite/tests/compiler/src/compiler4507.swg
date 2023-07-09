#global testerror
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var x = -128's8
    x = -x
    @print(x)
}