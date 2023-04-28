#global testerror
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    x := -128's8
    x = -x
    @print(x)
}