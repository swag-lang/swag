#global namespace Math
#global public

struct Vector2
{
    x, y: f32
}

impl Vector2
{
    const Zero:     Vector2{0, 0}
    const One:      Vector2{1, 1}
    const UnitX:    Vector2{1, 0}
    const UnitY:    Vector2{0, 1}
    const NegUnitX: Vector2{-1, 0}
    const NegUnitY: Vector2{0, -1}
    const Max:      Vector2{Swag.F32.Max, Swag.F32.Max}
    const NegMax:   Vector2{-Swag.F32.Max, -Swag.F32.Max}
    const Min:      Vector2{Swag.F32.Min, Swag.F32.Min}

    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y = value
    }

    mtd opEquals(other: Self)->bool
    {
        return x == other.x and y == other.y
    }

    mtd(op: string) opAssign(other: Self)
    {
        #if   op == "+="    { x += other.x; y += other.y; }
        #elif op == "-="    { x -= other.x; y -= other.y; }
        #elif op == "*="    { x *= other.x; y *= other.y; }
        #else               #assert(false, "unsupported assign operation '" ++ op ++ "'")
    }

    mtd(op: string) opAssign(value: f32)
    {
        #if   op == "+="    { x += value; y += value; }
        #elif op == "-="    { x -= value; y -= value; }
        #elif op == "*="    { x *= value; y *= value; }
        #else               #assert(false, "unsupported assign operation '" ++ op ++ "'")
    }

    mtd(op: string) opBinary(other: Self)->Self
    {
        #if   op == "+" return Vector2{x + other.x, y + other.y}
        #elif op == "-" return Vector2{x - other.x, y - other.y}
        #elif op == "*" return Vector2{x * other.x, y * other.y}
        #elif op == "/" return Vector2{x / other.x, y / other.y}
        #else           #assert(false, "unsupported binary operation '" ++ op ++ "'")
    }

    mtd(op: string) opBinary(value: f32)->Self
    {
        #if   op == "+" return Vector2{x + value, y + value}
        #elif op == "-" return Vector2{x - value, y - value}
        #elif op == "*" return Vector2{x * value, y * value}
        #elif op == "/" return Vector2{x / value, y / value}
        #else           #assert(false, "unsupported binary operation '" ++ op ++ "'")
    }

    // Set content
    #[Swag.Inline]
    mtd set(x, y: f32)
    {
        self.x = #scopefct.x
        self.y = #scopefct.y
    }

    // Return sthe length of the vector
    mtdc length() => sqrt((x * x) + (y * y))

    // Returns the squared length of the vector
    mtdc lengthSquared() => (x * x) + (y * y)

    // Returns the dot product between two vectors
    mtdc dot(other: Self) => (x * other.x) + (y * other.y)

    // Returns the perpendicular vector in the 2D plane.
    // 'axis' will give the sign of the result
    mtdc cross(axis: f32)->Vector2
    {
        return @{y * axis, -x * axis}
    }

    // Returns the Z signed length of the perpendicular vector
    mtdc cross(other: Vector2)->f32
    {
        return x * other.y - y * other.x
    }

    // Set the vector to zero
    mtd clear()
    {
        x = 0
        y = 0
    }

    // Negate the vector
    mtd negate()
    {
        x = -x
        y = -y
    }

    // Returns true if this Vector2 is null
    mtdc isZero()->bool
    {
        return x == 0 and y == 0
    }

    // Returns true if this Vector2 is null with an epsilon
    mtdc isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Returns true if this Vector2 is equals to another with an epsilon
    mtdc isEqualEpsilon(other: Vector2, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps)
    }

    // Returns the distance between two vectors
    mtdc distance(to: Self)->f32
    {
        xn := (x - to.x)
        yn := (y - to.y)
        return sqrt((xn * xn) + (yn * yn))
    }

    // Returns the square distance between two vectors
    mtdc distanceSquared(to: Self)->f32
    {
        xn := (x - to.x)
        yn := (y - to.y)
        return (xn * xn) + (yn * yn)
    }

    // Normalize this vector (set its length to 1)
    mtd normalize()
    {
        norm := sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized
    mtdc toNormalize()->Vector2
    {
        norm := sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Vector2{x * norm, y * norm}
    }

    // Set the vector length
    mtd setLengthSafe(newLength: f32)
    {
        normalizeSafe()
        x *= newLength
        y *= newLength
    }

    // Set the vector length
    mtd setLength(newLength: f32)
    {
        normalize()
        x *= newLength
        y *= newLength
    }

    // Normalize this vector (set its length to 1) even if its length is almost null
    mtd normalizeSafe()
    {
        norm := sqrt((x * x) + (y * y))
        if Math.isZeroEpsilon(norm)
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
    mtdc toNormalizeSafe()->Vector2
    {
        norm := sqrt((x * x) + (y * y))
        if Math.isZeroEpsilon(norm)
            return Zero
        norm = 1 / norm
        return Vector2{x * norm, y * norm}
    }

    // Returns a vector which is the 'min' of two vectors
    func min(a, b: Vector2)->Vector2
    {
        var result: retval = ?
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        return result
    }

    // Returns a vector which is the 'min' of two vectors
    func max(a, b: Vector2)->Vector2
    {
        var result: retval = ?
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        return result
    }
}