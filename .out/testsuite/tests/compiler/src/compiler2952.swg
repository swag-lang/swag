struct V2
{
    x, y: f32
}

impl V2
{
    #[Swag.inline]
    func set(using self, x, y: f32)
    {
        self.x = #scopefct.x
        self.y = #scopefct.y
    }
}

#[Swag.inline]
func test(cx, cy: f32)
{
    var v: V2
    v.set(cx, cy)
}

#test
{
    test(0, 1)
}