#global testerror "Err0040"

enum Rgb : u32
{
    R = 0xFF000000
}

struct Color
{
}

impl Color
{
    func opAffect(using self, c: Rgb)
    {
    }
}

var xx: Color = Rgb.R
