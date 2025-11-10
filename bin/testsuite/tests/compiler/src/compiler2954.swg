struct Color {}
impl Color
{
    #[Swag.Implicit]
    func opAffect(me, v: bool) {}
}

struct Painter {}
impl Painter
{
    func draw(me, col: Color, size: f32) {}
}

#test
{
    var cdc: Painter
    cdc.draw(true, 1.0)
}
