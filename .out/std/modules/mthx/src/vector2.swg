#global public
using core.math

struct Vector2
{
    x, y: Float
}

impl Vector2
{
    const Zero:     Vector2{0, 0}
    const One:      Vector2{1, 1}
    const UnitX:    Vector2{1, 0}
    const UnitY:    Vector2{0, 1}
    const UnitNX:   Vector2{-1, 0}
    const UnitNY:   Vector2{0, -1}

    #[Swag.complete]
    func opAffect(using self, value: Float)
    {
        x = value
        y = value
    }

    func opEquals(using self, other: Self)->bool
    {
        return x == other.x and y == other.y
    }

    func(op: string) opAssign(using self, other: Self)
    {
        #if   op == "+="    { x += other.x; y += other.y; }
        #elif op == "-="    { x -= other.x; y -= other.y; }
        #elif op == "*="    { x *= other.x; y *= other.y; }
        #else               #assert(false, "unsupported assign operation '" ~op~ "'")
    }

    func(op: string) opAssign(using self, value: Float)
    {
        #if   op == "+="    { x += value; y += value; }
        #elif op == "-="    { x -= value; y -= value; }
        #elif op == "*="    { x *= value; y *= value; }
        #else               #assert(false, "unsupported assign operation '" ~op~ "'")
    }

    func(op: string) opBinary(using self, other: Self)->Self
    {
        #if   op == "+" return Vector2{x + other.x, y + other.y}
        #elif op == "-" return Vector2{x - other.x, y - other.y}
        #elif op == "*" return Vector2{x * other.x, y * other.y}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    func(op: string) opBinary(using self, value: Float)->Self
    {
        #if   op == "+" return Vector2{x + value, y + value}
        #elif op == "-" return Vector2{x - value, y - value}
        #elif op == "*" return Vector2{x * value, y * value}
        #else           #assert(false, "unsupported binary operation '" ~op~ "'")
    }

    // Return sthe length of the vector
    func length(using self) => sqrt((x * x) + (y * y))

    // Returns the squared length of the vector
    func lengthSquared(using self) => (x * x) + (y * y)

    // Returns the dot product between two vectors
    func dot(using self, other: Self) => (x * other.x) + (y * other.y)

    // Returns the perpendicular vector. 'axis' will give the sign
    func cross(using self, axis: Float)->Vector2
    {
        return @{y * axis, -x * axis}
    }

    // Set the vector to zero
    func clear(using self)
    {
        x = 0
        y = 0
    }

    // Negate the vector
    func negate(using self)
    {
        x = -x
        y = -y
    }

    // Returns true if this Vector2 is null
    func isZero(using self)->bool
    {
        return x == 0 and y == 0
    }

    // Returns true if this Vector2 is null with an epsilon
    func isZeroEpsilon(using self, eps = cast(Float) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Returns the distance between two vectors
    func distance(using self, to: Self)->Float
    {
        xn := (x - to.x)
        yn := (y - to.y)
        return sqrt((xn * xn) + (yn * yn))
    }

    // Returns the square distance between two vectors
    func distanceSquared(using self, to: Self)->Float
    {
        xn := (x - to.x)
        yn := (y - to.y)
        return (xn * xn) + (yn * yn)
    }

    // Normalize this vector (set its length to 1)
    func normalize(using self)
    {
        norm := sqrt((x * x) + (y * y))
        debug.assert(!math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized
    func toNormalize(using self)->Vector2
    {
        norm := sqrt((x * x) + (y * y))
        debug.assert(!math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Vector2{x * norm, y * norm}
    }

    // Normalize this vector (set its length to 1) even if its length is almost null
    func normalizeSafe(using self)
    {
        norm := sqrt((x * x) + (y * y))
        if math.isZeroEpsilon(norm)
        {
            x = 0
            y = 0
            return
        }

        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized even if its length is almost null
    func toNormalizeSafe(using self)->Vector2
    {
        norm := sqrt((x * x) + (y * y))
        if math.isZeroEpsilon(norm)
            return Zero
        norm = 1 / norm
        return Vector2{x * norm, y * norm}
    }
}