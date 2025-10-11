struct Color
{
    b:   bool
}

impl Color
{
    //#[Swag.Implicit]
    func opAffect(me, v: bool)
    {
        me.b = v
    }
}

struct Painter {}
impl Painter
{
    func draw(me, col: Color, size: f32)
    {
        @assert(col.b == true)
    }
}

#test
{
    var cdc: Painter
    cdc.draw(cast(Color) true, 1.0)
}
