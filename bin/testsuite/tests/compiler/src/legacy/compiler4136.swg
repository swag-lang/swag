#global testerror "Saf0012"

#[Swag.Optimize("", false), Swag.Safety(.All, "math", true), Swag.Safety(.All, "nan", false)]
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
