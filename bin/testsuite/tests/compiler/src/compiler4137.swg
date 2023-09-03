#global testerror "Saf0026"

#[Swag.Optim("", false)]
#[Swag.Safety("math", true)]
#[Swag.Safety("nan", false)]
func toto(y: f32)
{
    var x = @asin(y)
    x = x
}

//#[Swag.PrintBc]
#test
{
    toto(-2)
}