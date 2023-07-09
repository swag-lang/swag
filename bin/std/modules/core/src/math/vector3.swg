#global namespace Math
#global public

#[Serialization.PodFinal]
struct Vector3
{
    x, y, z: f32
}

impl Vector3
{
    const Zero:     Self{0, 0, 0}
    const One:      Self{1, 1, 1}
    const UnitX:    Self{1, 0, 0}
    const UnitY:    Self{0, 1, 0}
    const UnitZ:    Self{0, 0, 1}
    const NegUnitX: Self{-1, 0, 0}
    const NegUnitY: Self{0, -1, 0}
    const NegUnitZ: Self{0, 0, -1}
    const Max:      Self{Swag.F32.Max, Swag.F32.Max, Swag.F32.Max}
    const NegMax:   Self{-Swag.F32.Max, -Swag.F32.Max, -Swag.F32.Max}
    const Min:      Self{Swag.F32.Min, Swag.F32.Min, Swag.F32.Min}

    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y, z = value
    }

    mtd opEquals(other: Self)->bool
    {
        return x == other.x and y == other.y and z == other.z
    }

    mtd(op: string) opAssign(other: Self)
    {
        #if   op == "+="    { x += other.x; y += other.y; z += other.z; }
        #elif op == "-="    { x -= other.x; y -= other.y; z -= other.z; }
        #elif op == "*="    { x *= other.x; y *= other.y; z *= other.z; }
        #elif op == "/="    { x /= other.x; y /= other.y; z /= other.z; }
        #else               #error "unsupported assign operation '" ++ op ++ "'"
    }

    mtd(op: string) opAssign(value: f32)
    {
        #if   op == "+="    { x += value; y += value; z += value; }
        #elif op == "-="    { x -= value; y -= value; z -= value; }
        #elif op == "*="    { x *= value; y *= value; z *= value; }
        #elif op == "/="    { x /= value; y /= value; z /= value; }
        #else               #error "unsupported assign operation '" ++ op ++ "'"
    }

    mtd(op: string) opBinary(other: Self)->Self
    {
        #if   op == "+" return Self{x + other.x, y + other.y, z + other.z}
        #elif op == "-" return Self{x - other.x, y - other.y, z - other.z}
        #elif op == "*" return Self{x * other.x, y * other.y, z * other.z}
        #elif op == "/" return Self{x / other.x, y / other.y, z / other.z}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtd(op: string) opBinary(value: f32)->Self
    {
        #if   op == "+" return Self{x + value, y + value, z + value}
        #elif op == "-" return Self{x - value, y - value, z - value}
        #elif op == "*" return Self{x * value, y * value, z * value}
        #elif op == "/" return Self{x / value, y / value, z / value}
        #else           #error "unsupported binary operation '" ++ op ++ "'"
    }

    mtd(op: string) opUnary()->Self
    {
        #if   op == "-" return Self{-x, -y, -z}
        #else #error "unsupported unary operation '" ++ op ++ "'"
    }

    // Set content
    #[Swag.Inline]
    mtd set(x, y, z: f32)
    {
        self.x = #self.x
        self.y = #self.y
        self.z = #self.z
    }

    // Return sthe length of the vector
    mtd const length() => sqrt((x * x) + (y * y) + (z * z))

    // Returns the squared length of the vector
    mtd const lengthSquared() => (x * x) + (y * y) + (z * z)

    // Returns the dot product between two vectors
    mtd const dot(other: Self) => (x * other.x) + (y * other.y) + (z * other.z)

    // Set the vector to zero
    mtd clear()
    {
        x, y, z = 0
    }

    // Negate the vector
    mtd negate()
    {
        x = -x
        y = -y
        z = -z
    }

    // Returns true if this vector is null
    mtd const isZero()->bool
    {
        return x == 0 and y == 0 and z == 0
    }

    // Returns true if this vector is null with an epsilon
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps) and isZeroEpsilon(z, eps)
    }

    // Returns true if this vector is equals to another with an epsilon
    mtd const isEqualEpsilon(other: Self, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps) and Math.isEqualEpsilon(z, other.z, eps)
    }

    // Returns the distance between two vectors
    mtd const distance(to: Self)->f32
    {
        var xn = (x - to.x)
        var yn = (y - to.y)
        var zn = (z - to.z)
        return sqrt((xn * xn) + (yn * yn) + (zn * zn))
    }

    // Returns the square distance between two vectors
    mtd const distanceSquared(to: Self)->f32
    {
        var xn = (x - to.x)
        var yn = (y - to.y)
        var zn = (z - to.z)
        return (xn * xn) + (yn * yn) + (zn * zn)
    }

    // Normalize this vector (set its length to 1)
    mtd normalize()
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
        z *= norm
    }

    // Returns this vector normalized
    mtd const toNormalize()->Self
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Self{x * norm, y * norm, z * norm}
    }

    // Set the vector length
    mtd setLengthSafe(newLength: f32)
    {
        normalizeSafe()
        x *= newLength
        y *= newLength
        z *= newLength
    }

    // Set the vector length
    mtd setLength(newLength: f32)
    {
        normalize()
        x *= newLength
        y *= newLength
        z *= newLength
    }

    // Normalize this vector (set its length to 1) even if its length is almost null
    mtd normalizeSafe()
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        if Math.isZeroEpsilon(norm)
        {
            x, y, z = 0
            return
        }

        norm = 1 / norm
        x *= norm
        y *= norm
        z *= norm
    }

    // Returns this vector normalized even if its length is almost null
    mtd const toNormalizeSafe()->Self
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        if Math.isZeroEpsilon(norm)
            return Zero
        norm = 1 / norm
        return Self{x * norm, y * norm, z * norm}
    }

    // Returns a vector which is the 'min' of two vectors
    func min(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        result.z = Math.min(a.z, b.z)
        return result
    }

    // Returns a vector which is the 'min' of two vectors
    func max(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        result.z = Math.max(a.z, b.z)
        return result
    }

    // Lerp one vector with another
    func lerp(src, dst: Vector3, factor: f32)->Vector3
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        result.z = Math.lerp(src.z, dst.z, factor)
        return result
    }
}