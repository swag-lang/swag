#global testerror "Saf0016"

func toto(x: f32) {}

#[Swag.Safety("nan", true)]
#test
{
    toto(Swag.F32.Nan)
}