#global testerror

func toto(x: f32) {}

#[Swag.Safety("nan", true)]
#test
{
    var x = 0'f32
    var y = x * Swag.F32.Inf
    toto(y)
}