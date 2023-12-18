#global public
#global namespace Math

// A simple Point with 2 coordinates X and Y
#[Serialization.PodFinal]
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

    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y = value
    }

    mtd const opEquals(other: Point)->bool
    {
        return x == other.x and y == other.y
    }

    mtd(op: string) opAssign(other: Point)
    {
        #if   op == "+="    { x += other.x; y += other.y; }
        #elif op == "-="    { x -= other.x; y -= other.y; }
        #elif op == "*="    { x *= other.x; y *= other.y; }
        #elif op == "/="    { x /= other.x; y /= other.y; }
        #else               #error "unsupported assign operation '" ++ op ++ "'"
    }

    mtd(op: string) opAssign(value: f32)
    {
        #if   op == "+="    { x += value; y += value; }
        #elif op == "-="    { x -= value; y -= value; }
        #elif op == "*="    { x *= value; y *= value; }
        #elif op == "/="    { x /= value; y /= value; }
        #else               #error "unsupported assign operation '" ++ op ++ "'"
    }

    mtd(op: string) const opBinary(other: Point)->Self
    {
        #if   op == "+" return Point{x + other.x, y + other.y}
        #elif op == "-" return Point{x - other.x, y - other.y}
        #elif op == "*" return Point{x * other.x, y * other.y}
        #elif op == "/" return Point{x / other.x, y / other.y}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtd(op: string) const opBinary(value: f32)->Self
    {
        #if   op == "+" return Point{x + value, y + value}
        #elif op == "-" return Point{x - value, y - value}
        #elif op == "*" return Point{x * value, y * value}
        #elif op == "/" return Point{x / value, y / value}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtd(op: string) const opUnary()->Self
    {
        var result: retval
		#if op == "-"
		{
			result.x = -x
			result.y = -y
			return result
		}
        #else
        {
            #error "unsupported unary operation '" ++ op ++ "'"
        }
    }

    // Set Point to (0, 0)
    mtd clear()
    {
        x, y = 0
    }

    // Returns 'true' if the Point is null
    mtd const isZero()->bool
    {
        return x == 0 and y == 0
    }

    // Check for zero with an epsilon
    mtd const isZeroEpsilon(eps = ConstF32.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Check if two points are equal with an epsilon
    mtd const isEqualEpsilon(other: Point, eps = ConstF32.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps)
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    mtd offset(value: f32)
    {
        x, y += value
    }

    // Offset this Point by a given value
    #[Swag.Overload]
    func offset(self, x, y: f32)
    {
        self.x += x
        self.y += y
    }

    // Perform a Math.round operation on all the coordinates
    mtd round()
    {
        x = acast round(x)
        y = acast round(y)
    }

    // Perform a Math.trunc operation on all the coordinates
    mtd trunc()
    {
        x = acast trunc(x)
        y = acast trunc(y)
    }

    // Perform a Math.ceil operation on all the coordinates
    mtd ceil()
    {
        x = acast ceil(x)
        y = acast ceil(y)
    }
}