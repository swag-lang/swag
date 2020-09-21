struct Argb
{
    a, r, g, b: u8
}

impl Argb
{
    func extract(self) -> struct{f32,f32,f32,f32}
    {
        a := self.a / 255.0'f32
        r := self.r / 255.0'f32
        g := self.g / 255.0'f32
        b := self.b / 255.0'f32
        return @{a, r, g, b}
    }
}

#test
{
    color := Argb(255, 255, 128, 0)
    (a, r, g, b) := color.extract()
    @assert(a == 1)
    @assert(r == 1)
    @assert(0 < g < 1)
    /*@print(cast(f64) a); @print(" ")
    @print(cast(f64) r); @print(" ")
    @print(cast(f64) g); @print(" ")
    @print(cast(f64) b); @print(" ")*/
}