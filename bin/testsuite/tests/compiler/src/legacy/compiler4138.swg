#global testerror "Saf0009"

#[Swag.Optimize("", false), Swag.Safety(.All, "math", true), Swag.Safety(.All, "nan", false)]
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
