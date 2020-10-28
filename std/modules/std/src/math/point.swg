public namespace std.math;

// A simple Point with 2 coordinates X and Y
struct Point
{
    x: f32
    y: f32
}

impl Point
{
    #[swag.complete]
    func opAffect(using self, value: f32)
    {
        x, y = value
    }

    func opEquals(using self, other: Self)->bool
    {
        return x == other.x && y == other.y
    }

    func(op: string) opAssign(using self, other: Self)
    {
        #if   op == "+="    { x += other.x; y += other.y; }
        #elif op == "-="    { x -= other.x; y -= other.y; }
        #elif op == "*="    { x *= other.x; y *= other.y; }
        #else               #assert(false, "unsupported assign operation '" ~op~ "'")
    }

    func(op: string) opAssign(using self, value: f32)
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

    func(op: string) opBinary(using self, value: f32)->Self
    {
        #if   op == "+" return Self{x + value, y + value}
        #elif op == "-" return Self{x - value, y - value}
        #elif op == "*" return Self{x * value, y * value}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    // Represents a Point with all coordinates to zero
    const Zero: Self{0, 0}
    // Represents a Point with all coordinates to one
    const One: Self{1, 1}

    // Set PointPriv to (0, 0)
    func clear(using self)
    {
        x, y = 0
    }

    // Returns 'true' if the Point is null
    func isZero(using self)->bool
    {
        return x == y == 0
    }

    // Offset this Point by a given value
    func offset(using self, value: f32)
    {
        x, y += value
    }

    // Offset this Point by a given value
    func offset(using self, valueX, valueY: f32)
    {
        x += valueX
        y += valueY
    }

    // Check for zero with an epsilon
    func isZeroEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return math.isZeroEpsilon(x, eps) && math.isZeroEpsilon(y, eps)
    }

    // Transform a PointF to a Point
    func round(using self)
    {
        x = acast math.round(x)
        y = acast math.round(y)
    }

    // Transform a PointF to a Point
    func trunc(using self)
    {
        x = acast math.trunc(x)
        y = acast math.trunc(y)
    }
}