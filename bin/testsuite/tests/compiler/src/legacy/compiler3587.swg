struct Color {}

impl Color
{
    mtd toHsl()->{ h: f32, s: f32, l: f32 }
    {
        return {1, 2, 3}
    }
}

var c1: Color

#test
{
    let (?, sat, lum) = c1.toHsl()
    @assert(sat == 2)
    @assert(lum == 3)
}
