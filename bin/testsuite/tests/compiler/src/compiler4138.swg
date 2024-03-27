#global testerror "Saf0009"

#[Swag.Optim("", false), Swag.Safety("math", true), Swag.Safety("nan", false)]
func toto(y: f64)
{
    var x = @acos(y)
    x = x
}

//#[Swag.PrintBc]
#test
{
    toto(-2)
}
