#global namespace Math
#global public

#[Serialization.PodFinal]
struct Vector2
{
    x, y: f32
}

impl Vector2
{
    const Zero:     Self{0, 0}
    const One:      Self{1, 1}
    const UnitX:    Self{1, 0}
    const UnitY:    Self{0, 1}
    const NegUnitX: Self{-1, 0}
    const NegUnitY: Self{0, -1}
    const Max:      Self{Swag.F32.Max, Swag.F32.Max}
    const NegMax:   Self{-Swag.F32.Max, -Swag.F32.Max}
    const Min:      Self{Swag.F32.Min, Swag.F32.Min}

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

    mtdc(op: string) opBinary(other: Self)->Self
    {
        #if   op == "+" return Self{x + other.x, y + other.y}
        #elif op == "-" return Self{x - other.x, y - other.y}
        #elif op == "*" return Self{x * other.x, y * other.y}
        #elif op == "/" return Self{x / other.x, y / other.y}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtdc(op: string) opBinary(value: f32)->Self
    {
        #if   op == "+" return Self{x + value, y + value}
        #elif op == "-" return Self{x - value, y - value}
        #elif op == "*" return Self{x * value, y * value}
        #elif op == "/" return Self{x / value, y / value}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtdc(op: string) opUnary()->Self
    {
        #if   op == "-" return Self{-x, -y}
        #else #error "unsupported unary operation '" ++ op ++ "'"
    }

    // Set content
    #[Swag.Inline]
    mtd set(x, y: f32)
    {
        self.x = #self.x
        self.y = #self.y
    }

    // Return sthe length of the vector
    mtdc length() => sqrt((x * x) + (y * y))

    // Returns the squared length of the vector
    mtdc lengthSquared() => (x * x) + (y * y)

    // Returns the dot product between two vectors
    mtdc dot(other: Self) => (x * other.x) + (y * other.y)

    // Returns the perpendicular vector in the 2D plane.
    // 'axis' will give the sign of the result
    mtdc cross(axis: f32)->Self
    {
        return {y * axis, -x * axis}
    }

    // Returns the Z signed length of the perpendicular vector
    mtdc cross(other: Self)->f32
    {
        return x * other.y - y * other.x
    }

    // Set the vector to zero
    mtd clear()
    {
        x, y = 0
    }

    // Negate the vector
    mtd negate()
    {
        x = -x
        y = -y
    }

    // Returns true if this vector is null
    mtdc isZero()->bool
    {
        return x == 0 and y == 0
    }

    // Returns true if this vector is null with an epsilon
    mtdc isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Returns true if this vector is equals to another with an epsilon
    mtdc isEqualEpsilon(other: Self, eps = cast(f32) ConstF64.Epsilon)->bool
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
    mtdc toNormalize()->Self
    {
        norm := sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Self{x * norm, y * norm}
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
            x, y = 0
            return
        }

        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized even if its length is almost null
    mtdc toNormalizeSafe()->Self
    {
        norm := sqrt((x * x) + (y * y))
        if Math.isZeroEpsilon(norm)
            return Zero
        norm = 1 / norm
        return Self{x * norm, y * norm}
    }

    // Returns a vector which is the 'min' of two vectors
    func min(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        return result
    }

    // Returns a vector which is the 'min' of two vectors
    func max(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        return result
    }

    // Math.floor
    mtd floor()
    {
        x = Math.floor(x)
        y = Math.floor(y)
    }

    // Math.ceil
    mtd ceil()
    {
        x = Math.ceil(x)
        y = Math.ceil(y)
    }

    // Math.round
    mtd round()
    {
        x = Math.round(x)
        y = Math.round(y)
    }

    // Rotate the vector by a given angle
    mtd rotate(angle: Math.Angle)
    {
        s := sin(angle.rad)
        c := cos(angle.rad)
        tx := x
        ty := y
        x = (c * tx) - (s * ty)
        y = (s * tx) + (c * ty)
    }

    // Returns the cosine angle with another vector
    mtdc cosAngle(other: Self)->f32
    {
        vec1 := toNormalize()
        vec2 := other.toNormalize()
        return vec1.dot(vec2)
    }

    // Returns the angle with another vector
    mtdc angle(other: Self)->f32
    {
        angle := Math.acos(cosAngle(other))
        //sign  := (x * other.y) - (y * other.x)
        //if sign < 0 angle = -angle;
        return angle
    }

    // Lerp one vector with another
    func lerp(src, dst: Vector2, factor: f32)->Vector2
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        return result
    }
}