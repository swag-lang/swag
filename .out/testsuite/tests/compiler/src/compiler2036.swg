struct Argb2036
{
    a, r, g, b: u8
}

impl Argb2036
{
    //#[Swag.inline]
    func extract(self) -> {f32,f32,f32,f32}
    {
        _a := self.a / 255.0'f32
        _r := self.r / 255.0'f32
        _g := self.g / 255.0'f32
        _b := self.b / 255.0'f32
        return @{_a, _r, _g, _b}
    }
}

#test
{
    color := Argb2036{255, 255, 128, 0}
    (a, r, g, b) := color.extract()
    @assert(a == 1)
    @assert(r == 1)
    @assert(0 < g and g < 1)
    /*@print(cast(f64) a); @print(" ")
    @print(cast(f64) r); @print(" ")
    @print(cast(f64) g); @print(" ")
    @print(cast(f64) b); @print(" ")*/
}