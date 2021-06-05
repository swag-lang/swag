
#global testerror

//#[Swag.printbc]
#[Swag.safety("", true)]
#test
{
    x := [10]
    y := 1
    z := x[y]
}