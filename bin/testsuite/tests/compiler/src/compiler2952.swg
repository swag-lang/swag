struct V2 { x, y: f32 }

impl V2
{
    #[Swag.Inline]
    func set(me, x, y: f32)
    {
        me.x = x
        me.y = y
    }
}

#[Swag.Inline]
func test(cx, cy: f32)
{
    var v: V2
    v.set(cx, cy)
}

#test
{
    test(0, 1)
}
