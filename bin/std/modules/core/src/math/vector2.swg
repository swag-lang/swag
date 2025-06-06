#global namespace Math
#global public

#[Serialization.Final]
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

    #[Swag.Complete, Swag.Inline]
    mtd opAffect(value: f32)
    {
        x, y = value
    }

    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(other: Self)
    {
        #if op == "+="
        {
            x += other.x
            y += other.y
        }
        #elif op == "-="
        {
            x -= other.x
            y -= other.y
        }
        #elif op == "*="
        {
            x *= other.x
            y *= other.y
        }
        #elif op == "/="
        {
            x /= other.x
            y /= other.y
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(value: f32)
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
        #elif op == "/="
        {
            x /= value
            y /= value
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(other: Self)->Self
    {
        #if op == "+":
            return Self{x + other.x, y + other.y}
        #elif op == "-":
            return Self{x - other.x, y - other.y}
        #elif op == "*":
            return Self{x * other.x, y * other.y}
        #elif op == "/":
            return Self{x / other.x, y / other.y}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(value: f32)->Self
    {
        #if op == "+":
            return Self{x + value, y + value}
        #elif op == "-":
            return Self{x - value, y - value}
        #elif op == "*":
            return Self{x * value, y * value}
        #elif op == "/":
            return Self{x / value, y / value}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    #[Swag.Inline]
    mtd(op: string) const opUnary()->Self
    {
        #if op == "-":
            return Self{-x, -y}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set content
    #[Swag.Inline]
    mtd set(x, y: f32)
    {
        self.x = #self.x
        self.y = #self.y
    }

    // Return sthe length of the vector
    #[Swag.Inline]
    mtd const length()->f32 => sqrt((x * x) + (y * y))

    // Returns the squared length of the vector
    #[Swag.Inline]
    mtd const lengthSquared()->f32 => (x * x) + (y * y)

    // Returns the dot product between two vectors
    #[Swag.Inline]
    mtd const dot(other: Self)->f32 => (x * other.x) + (y * other.y)

    // Returns the perpendicular vector in the 2D plane.
    // 'axis' will give the sign of the result
    #[Swag.Overload, Swag.Inline]
    mtd const cross(axis: f32)->Self
    {
        return {y * axis, -x * axis}
    }

    // Returns the Z signed length of the perpendicular vector
    #[Swag.Overload, Swag.Inline]
    mtd const cross(other: Self)->f32
    {
        return x * other.y - y * other.x
    }

    // Set the vector to zero
    #[Swag.Inline]
    mtd clear()
    {
        x, y = 0
    }

    // Negate the vector
    #[Swag.Inline]
    mtd negate()
    {
        x = -x
        y = -y
    }

    // Returns true if this vector is null
    #[Swag.Inline]
    mtd const isZero()->bool
    {
        return x == 0 and y == 0
    }

    // Returns true if this vector is null with an epsilon
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Returns true if this vector is equals to another with an epsilon
    mtd const isEqualEpsilon(other: Self, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps)
    }

    // Returns the distance between two vectors
    mtd const distance(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        return sqrt((xn * xn) + (yn * yn))
    }

    // Returns the square distance between two vectors
    mtd const distanceSquared(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        return (xn * xn) + (yn * yn)
    }

    // Normalize this vector (set its length to 1)
    mtd normalize()
    {
        var norm = sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized
    mtd const toNormalize()->Self
    {
        var norm = sqrt((x * x) + (y * y))
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
        var norm = sqrt((x * x) + (y * y))
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
    mtd const toNormalizeSafe()->Self
    {
        var norm = sqrt((x * x) + (y * y))
        if Math.isZeroEpsilon(norm):
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
        let s  = sin(angle.rad)
        let c  = cos(angle.rad)
        let tx = x
        let ty = y
        x = (c * tx) - (s * ty)
        y = (s * tx) + (c * ty)
    }

    // Returns the cosine angle with another vector
    mtd const cosAngle(other: Self)->f32
    {
        return dot(other) / (length() * other.length())
    }

    // Returns the angle with another vector
    mtd const angle(other: Self)->f32
    {
        return Math.acos(cosAngle(other))
    }

    // Returns the angle with another vector in the range [0, 2Pi]
    mtd const angle0To2Pi(other: Self)->f32
    {
        var angle = Math.acos(cosAngle(other))
        let sign  = (x * other.y) - (y * other.x)
        if sign > 0:
            angle = ConstF32.TwoPi - angle
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
