#global testerror

#[Swag.Safety("math", true)]
func toto(y: f32)
{
    var z = @sqrt(y)
    @print(z)
}

#test
{
    toto(-1.0)
}