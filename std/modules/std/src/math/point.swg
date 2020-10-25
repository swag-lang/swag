namespace std.math;

// A simple Point with 2 coordinates X and Y
struct(T) PointBase
{
    x: T
    y: T
}

impl PointBase
{
    #[swag.complete]
    func opAffect(using self, value: T)
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

    func(op: string) opBinary(using self, value: T)->Self
    {
        #if   op == "+" return Self{x + value, y + value}
        #elif op == "-" return Self{x - value, y - value}
        #elif op == "*" return Self{x * value, y * value}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    // Represents a Point with all coordinates to zero
    const Zero: Self'T{0, 0}
    // Represents a Point with all coordinates to one
    const One: Self'T{1, 1}

    // Set PointBase to (0, 0)
    func clear(using self)
    {
        x, y = 0
    }

    // Returns 'true' if the Point is null
    func isZero(using self)->bool
    {
        return x == y == 0
    }

    // Returns 'true' if the Point is null with an epsilon
    func isZeroEpsilon(using self, eps: f32 = constF32.Epsilon)->bool
    {
        return math.isZeroEpsilon(x, eps) && math.isZeroEpsilon(y, eps)
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

public #bake Point = PointBase'f32
//public #bake PointS32 = PointBase's32