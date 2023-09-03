#global testerror "Saf0022"

#[Swag.Safety("math", true)]
func toto(y: f32)
{
    let z = @sqrt(y)
    @print(z)
}

#test
{
    toto(-1.0)
}