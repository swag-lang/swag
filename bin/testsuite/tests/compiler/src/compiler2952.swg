struct V2
{
    x, y: f32
}

impl V2
{
    #[Swag.Inline]
    func set(using self, x, y: f32)
    {
        self.x = #self.x
        self.y = #self.y
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