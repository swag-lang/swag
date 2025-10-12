#global testerror "Saf0014"

#[Swag.Safety(.All, "math", true)]
func toto(y: f32)
{
    let z = @sqrt(y)
    @print(z)
}

#test
{
    toto(-1.0)
}
