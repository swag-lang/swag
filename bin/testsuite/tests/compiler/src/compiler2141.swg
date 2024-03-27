struct(T) PointBase
{
    x, y: T
}

impl PointBase
{
    func(op: string) opAssign(using self, value: T)
    {
        #if op == "+="
        {
            x += value
            y += value
        }
        #elif op == "-="
        {
            x -= value
            y -= value
        }
        #elif op == "*="
        {
            x *= value
            y *= value
        }
        #else #do
            #error "unsupported assign operation '" ++ op ++ "'"
    }

    func(op: string) opBinary(using self, other: Self)->Self
    {
        #if op == "+" #do
            return Self{x + other.x, y + other.y}
        #elif op == "-" #do
            return Self{x - other.x, y - other.y}
        #elif op == "*" #do
            return Self{x * other.x, y * other.y}
        #else #do
            #error "unsupported binary operation '" ++ op ++ "'"
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    func offset(using self, value: T)
    {
        x, y += value
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    func offset(using self, valueX, valueY: T)
    {
        x += valueX
        y += valueY
    }
}

typealias Point2141 = PointBase'f32

#test
{
    var pt0, pt1, pt2: Point2141
    pt1.x = 1
    pt1.y = 1
    pt2.x = 0.5
    pt2.y = 1.5
    pt0   = pt1 + pt2
    @assert(pt0.x == 1.5)
    @assert(pt0.y == 2.5)
}
