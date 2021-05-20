
#global testerror

//#[Swag.printbc]
#[Swag.safety("", true)]
#test
{
    x := 10
    x = x % 0
}