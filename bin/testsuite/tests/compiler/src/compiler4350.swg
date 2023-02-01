//#global skip

struct(X1) Point
{
    x: X1
}

impl Point
{
    #[Swag.Inline]
    mtd mixup() -> Point
    {
        v := x
        var t: Point{v}
        return t
    }
}

#test
{
    p1 := Point{5's32}
    p3 := p1.mixup()

}