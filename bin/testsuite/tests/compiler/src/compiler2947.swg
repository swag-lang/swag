#global testerror "Err0043"

enum Rgb: u32
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

struct AA
{
    x: Color = Rgb.R
}
/*
//var xx: Color = Rgb.R

#[Swag.PrintBc]
#test
{
    var e: AA
}*/