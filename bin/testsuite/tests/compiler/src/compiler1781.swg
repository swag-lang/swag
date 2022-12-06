#global testerror

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    x := [10]
    y := 1
    z := x[y]
}