//#global skip

struct(X1) Point
{
    x:   X1
}

impl Point
{
    #[Swag.Inline]
    mtd mixup()->Point
    {
        let v  = me.x
        var t: Point{v}
        return t
    }
}

#test
{
    var p1 = Point{5's32}
    var p3 = p1.mixup()
}
