namespace std.math
{
    // A simple point with 2 float coordinates X and Y
	public struct Point
	{
		x: f32
        y: f32
	}

	public impl Point
	{
        #[swag.complete]
        func opAffect(self, value: f32)
        {
            self.x = value
            self.y = value
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
            #if   op == "+" return Point(x + other.x, y + other.y)
            #elif op == "-" return Point(x - other.x, y - other.y)
            #elif op == "*" return Point(x * other.x, y * other.y)
            #else           #assert(false, "unsupported binary operation '" ~op~ "'")
        }

        func(op: string) opBinary(using self, value: f32)->Self
        {
            #if   op == "+" return Point(x + value, y + value)
            #elif op == "-" return Point(x - value, y - value)
            #elif op == "*" return Point(x * value, y * value)
            #else           #assert(false, "unsupported binary operation '" ~op~ "'")
        }

        // Represents a Point with all coordinates to zero
        const Zero: Point{0, 0}
        // Represents a Point with all coordinates to one
        const One: Point{1, 1}

        // Set Point to (0, 0)
        func clear(using self)
        {
            x, y = 0
        }

        // Returns `true` if the Point is `null`
        func isZero(using self)->bool
        {
            return x == 0 && y == 0
        }

        // Returns `true` if the Point is `null` with an epsilon
        func isZeroEpsilon(using self, eps: f32 = constF32.Epsilon)->bool
        {
            return isZeroEpsilon(x, eps) && isZeroEpsilon(y, eps)
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
    }
}
