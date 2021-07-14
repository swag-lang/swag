#[Swag.Inline]
//#[Swag.PrintBc]
func(T) clamp(x, low, up: T) => x < low ? low : (x > up ? up : x)

#[Swag.Inline]
//#[Swag.PrintBc]
func toto(r: f32)
{
    var y: u8 = acast (clamp(r, 0, 1) * 255)
    @assert(cast(s64) y == 255)
}

//#[Swag.PrintBc]
#test
{
    toto(1)
}