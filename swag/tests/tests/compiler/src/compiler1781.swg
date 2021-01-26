
#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#test
{
    x := [10]
    y := 1
    z := x[y]
}