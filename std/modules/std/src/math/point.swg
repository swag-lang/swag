public namespace std.math;

// A simple Point with 2 coordinates X and Y
struct Point
{
    x: f32
    y: f32
}

impl Point
{
    // Represents a Point with all coordinates to zero
    const Zero: Point{0, 0}
    // Represents a Point with all coordinates to one
    const One:  Point{1, 1}

    #[swag.complete]
    func opAffect(using self, value: f32)
    {
        x, y = value
    }

    func opEquals(using self, other: Point)->bool
    {
        return x == other.x && y == other.y
    }

    func(op: string) opAssign(using self, other: Point)
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

    func(op: string) opBinary(using self, other: Point)->Self
    {
        #if   op == "+" return Point{x + other.x, y + other.y}
        #elif op == "-" return Point{x - other.x, y - other.y}
        #elif op == "*" return Point{x * other.x, y * other.y}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    func(op: string) opBinary(using self, value: f32)->Self
    {
        #if   op == "+" return Point{x + value, y + value}
        #elif op == "-" return Point{x - value, y - value}
        #elif op == "*" return Point{x * value, y * value}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    // Set Point to (0, 0)
    func clear(using self)
    {
        x, y = 0
    }

    // Returns 'true' if the Point is null
    func isZero(using self)->bool
    {
        return x == y == 0
    }

    // Check for zero with an epsilon
    func isZeroEpsilon(using self, eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps)
    }

    // Offset this Point by a given value
    func offset(using self, value: f32)
    {
        x, y += value
    }

    // Offset this Point by a given value
    func offset(self, x, y: f32)
    {
        self.x += x
        self.y += y
    }

    // Perform a math.round operation on all the coordinates
    func round(using self)
    {
        x = autocast round(x)
        y = autocast round(y)
    }

    // Perform a math.trunc operation on all the coordinates
    func trunc(using self)
    {
        x = autocast trunc(x)
        y = autocast trunc(y)
    }

    // Perform a math.ceil operation on all the coordinates
    func ceil(using self)
    {
        x = autocast ceil(x)
        y = autocast ceil(y)
    }
}