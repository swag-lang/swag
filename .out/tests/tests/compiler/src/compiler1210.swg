#[Swag.inline]
func lerp(x, y, factor: f32)->f32 => ((1 - factor) * x) + (factor * y)

//#[Swag.printbc]
#test
{
    x := lerp(0, 2, 0.5)
    @assert(x == 1)
}