struct Argb2036 { a, r, g, b: u8 }

impl Argb2036
{
    //#[Swag.Inline]
    func extract(me)->{ a, b, c, d: f32 }
    {
        var _a = me.a / 255.0'f32
        var _r = me.r / 255.0'f32
        var _g = me.g / 255.0'f32
        var _b = me.b / 255.0'f32
        return {_a, _r, _g, _b}
    }
}

#test
{
    var color = Argb2036{255, 255, 128, 0}
    let (a, r, g, b) = color.extract()
    @assert(a == 1)
    @assert(r == 1)
    @assert(0 < g and g < 1)
    /*@print(cast(f64) a); @print(" ")
    @print(cast(f64) r); @print(" ")
    @print(cast(f64) g); @print(" ")
    @print(cast(f64) b); @print(" ")*/
}
