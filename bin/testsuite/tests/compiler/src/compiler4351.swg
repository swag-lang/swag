//#global skip

struct(X1) Point
{
    x: bool
    y: X1
}

impl Point
{
    #[Swag.Inline]
    mtd mixup() -> Point
    {
        v := y
        var t: Point{true, v}
        return t
    }
}

#test
{
    p1 := Point{true, 5's32}
    p3 := p1.mixup()

}