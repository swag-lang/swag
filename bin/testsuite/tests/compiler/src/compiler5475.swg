//#global skip

struct(X1) Point
{
    x:   X1
}

impl Point
{
    #[Swag.Inline]
    mtd(X2) mixup(other: Point'X2)->Point'X2
    {
        return Point{x: other.x}
    }
}

#test
{
    var p1 = Point{5's32}
    var p2 = Point{"Hello"}
    let p3 = p1.mixup(p2)
    @assert(p3.x == "Hello")
}
