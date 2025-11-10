//#global skip

struct(X1, Y1) Point
{
    x:     X1
    y:     Y1
}

impl Point
{
    mtd(X2, Y2) mixup(other: Point'(X2, Y2))->Point'(X2, Y1)
    {
        return Point'(X2, Y1){x: other.x, y: me.y}
    }
}

#test
{
    var p1 = Point{x: 5's32, y: 10.4'f32}
    var p2 = Point{x: "Hello", y: 'c''rune}
    let p3 = p1.mixup(p2)

    @assert(#typeof(p3.x) == string)
    @assert(#typeof(p3.y) == f32)
}
