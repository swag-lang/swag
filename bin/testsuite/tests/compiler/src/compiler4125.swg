#global testerror "Saf0021"

func toto(x: f32) {}

#[Swag.Safety("nan", true)]
#test
{
    toto(Swag.F32.Nan)
}