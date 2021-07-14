
#global testerror

//#[Swag.PrintBc]
#[Swag.safety("", true)]
#test
{
    x := 10
    x = x % 0
}