#global testerror "Err0549"

enum Rgb: u32
{
    R = 0xFF000000
}

struct Color {}

impl Color
{
    func opAffect(me, c: Rgb) {}
}

struct AA
{
    x:   Color = Rgb.R
}
/*
//var xx: Color = Rgb.R

#[Swag.PrintBc]
#test
{
    var e: AA
}*/
