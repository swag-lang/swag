#global testerror "Saf0021"

func toto(x: f32) {}

#[Swag.Safety("nan", true)]
#test
{
    let x = 0'f32
    let y = x * Swag.F32.Inf
    toto(y)
}