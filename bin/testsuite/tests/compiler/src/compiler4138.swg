#global testerror

#[Swag.Optim("", false)]
#[Swag.Safety("math", true)]
#[Swag.Safety("nan", false)]
func toto(y: f64)
{
    x := @acos(y)
    x = x
}

//#[Swag.PrintBc]
#test
{
    toto(-2)
}