//#global skip

struct(X1) Point
{
    x: X1
}

impl Point
{
    mtd(X2) mixup(other: Point'X2) -> Point'X2
    {
        return Point{x = other.x}
    }
}

#test
{
    p1 := Point{5's32}
    p2 := Point{"Hello"}
    p3 := p1.mixup(p2)
    @assert(p3.x == "Hello")
}