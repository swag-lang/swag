struct Color {}
impl Color
{
    #[Swag.implicit]
    func opAffect(using self, v: bool)
    {
    }
}

struct Painter{}
impl Painter
{
    func draw(using self, col: Color, size: f32) {}
}

#test
{
    var cdc: Painter
    cdc.draw(true, 1.0)
}