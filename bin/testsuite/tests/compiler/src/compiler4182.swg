//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#[Swag.Safety("sanity", true)]
func toto()->f32
{
    y := -10'f32
    y *= 2
    y -= 1
    y += 32
    x := @sqrt(y)
    return x
}
