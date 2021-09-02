// A simple point with 2 float coordinates X and Y
struct(T) PointBase
{
    x, y: T
}

impl PointBase
{
    func(op: string) opAssign(using self, value: T)
    {
        #if   op == "+="    { x += value; y += value; }
        #elif op == "-="    { x -= value; y -= value; }
        #elif op == "*="    { x *= value; y *= value; }
        #else               #assert(false, "unsupported assign operation '" ~op~ "'")
    }

    func(op: string) opBinary(using self, other: Self)->Self
    {
        #if   op == "+" return Self{x + other.x, y + other.y}
        #elif op == "-" return Self{x - other.x, y - other.y}
        #elif op == "*" return Self{x * other.x, y * other.y}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    // Offset this Point by a given value
    func offset(using self, value: T)
    {
        x, y += value
    }

    // Offset this Point by a given value
    func offset(using self, valueX, valueY: T)
    {
        x += valueX
        y += valueY
    }
}

alias Point2141 = PointBase'f32

#test
{
    var pt0, pt1, pt2: Point2141
    pt1.x = 1; pt1.y = 1
    pt2.x = 0.5; pt2.y = 1.5
    pt0 = pt1 + pt2
    @assert(pt0.x == 1.5)
    @assert(pt0.y == 2.5)
}