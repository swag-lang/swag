#global testerror "Saf0017"

func toto(x: f32) {}

#[Swag.Safety(.All, "nan", true)]
#test
{
    toto(Swag.F32.NaN)
}
