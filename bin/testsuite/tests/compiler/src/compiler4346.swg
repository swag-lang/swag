//#global skip

struct(X1, Y1) Point
{
    x:     X1
    y:     Y1
}

impl Point
{
    mtd(X2, Y2) mixup(other: Point'(X2, Y2))->Point'(X1, Y2)
    {
        return Point'(X1, Y2){x: me.x, y: other.y}
    }
}

#test
{
    var p1 = Point{x: 5's32, y: 10.4'f32}
    var p2 = Point{x: "Hello", y: 'c''rune}
    let p3 = p1.mixup'(string, rune)(p2)

    @assert(#typeof(p3.x) == s32)
    @assert(#typeof(p3.y) == rune)
}
