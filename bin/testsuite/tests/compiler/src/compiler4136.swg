#global testerror "Saf0025"

#[Swag.Optim("", false)]
#[Swag.Safety("math", true)]
#[Swag.Safety("nan", false)]
func toto(y: f32)
{
    var x = @log10(y)
    x = x
}

//#[Swag.PrintBc]
#test
{
    toto(-2)
}