#global testerror "Saf0010"

#[Swag.Optimize(.All, false), Swag.Safety(.All, .Math, true), Swag.Safety(.All, .NaN, false)]
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
