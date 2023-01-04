#global testerror

func toto(x: f32) {}

#[Swag.Safety("nan", true)]
#test
{
    x := 0'f32
    y := x * Swag.F32.Inf
    toto(y)
}