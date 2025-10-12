//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#[Swag.Sanity(true)]
func toto()->f32
{
    var y = -10'f32
    y *= 2
    y -= 1
    y += 32
    let x = @sqrt(y)
    return x
}
