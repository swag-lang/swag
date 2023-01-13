#global testerror

#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    y := -10'f32
    y *= 2
    y -= 1
    x := @sqrt(y)
}
