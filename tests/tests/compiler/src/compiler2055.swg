#[swag.inline]
//#[swag.printbc]
func(T) clamp(x, low, up: T) => x < low ? low : (x > up ? up : x)

#[swag.inline]
//#[swag.printbc]
func toto(r: f32)
{
    var y: u8 = autocast (clamp(r, 0, 1) * 255)
    @assert(cast(s64) y == 255)
}

//#[swag.printbc]
#test
{
    toto(1)
}