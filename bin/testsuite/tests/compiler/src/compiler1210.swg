#[Swag.Inline]
func lerp(x, y, factor: f32)->f32 => ((1 - factor) * x) + (factor * y)

//#[Swag.PrintBc]
#test
{
    let x = lerp(0, 2, 0.5)
    @assert(x == 1)
}