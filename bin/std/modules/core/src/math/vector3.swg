#global namespace Math
#global public

// A 3D vector struct representing a point or direction in 3D space.
// Contains x, y, and z components as 32-bit floating point values.
#[Serialization.Final]
struct Vector3
{
    x, y, z: f32
     // X, Y, and Z components of the vector
}

impl Vector3
{
    // Common vector constants
    const Zero:     Self{0, 0, 0}                                         // Zero vector (0, 0, 0)
    const One:      Self{1, 1, 1}                                         // Unit vector in all directions (1, 1, 1)
    const UnitX:    Self{1, 0, 0}                                         // Unit vector along X-axis (1, 0, 0)
    const UnitY:    Self{0, 1, 0}                                         // Unit vector along Y-axis (0, 1, 0)
    const UnitZ:    Self{0, 0, 1}                                         // Unit vector along Z-axis (0, 0, 1)
    const NegUnitX: Self{-1, 0, 0}                                        // Negative unit vector along X-axis (-1, 0, 0)
    const NegUnitY: Self{0, -1, 0}                                        // Negative unit vector along Y-axis (0, -1, 0)
    const NegUnitZ: Self{0, 0, -1}                                        // Negative unit vector along Z-axis (0, 0, -1)
    const Max:      Self{Swag.F32.Max, Swag.F32.Max, Swag.F32.Max}        // Maximum possible vector values
    const NegMax:   Self{-Swag.F32.Max, -Swag.F32.Max, -Swag.F32.Max}     // Negative maximum vector values
    const Min:      Self{Swag.F32.Min, Swag.F32.Min, Swag.F32.Min}        // Minimum possible vector values

    // Set all components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set all components to 5.0.
    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y, z = value
    }

    // Compound assignment operators for vector-vector operations (+=, -=, *=, /=).
    // Performs component-wise operations with another vector.
    #[Swag.Overload]
    mtd(op: string) opAssign(other: Self)
    {
        #if op == "+="
        {
            x += other.x
            y += other.y
            z += other.z
        }
        #elif op == "-="
        {
            x -= other.x
            y -= other.y
            z -= other.z
        }
        #elif op == "*="
        {
            x *= other.x
            y *= other.y
            z *= other.z
        }
        #elif op == "/="
        {
            x /= other.x
            y /= other.y
            z /= other.z
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Compound assignment operators for vector-scalar operations (+=, -=, *=, /=).
    // Applies the scalar 'value' to all components of the vector.
    #[Swag.Overload]
    mtd(op: string) opAssign(value: f32)
    {
        #if op == "+="
        {
            x += value
            y += value
            z += value
        }
        #elif op == "-="
        {
            x -= value
            y -= value
            z -= value
        }
        #elif op == "*="
        {
            x *= value
            y *= value
            z *= value
        }
        #elif op == "/="
        {
            x /= value
            y /= value
            z /= value
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-vector operations (+, -, *, /).
    // Performs component-wise operations between two vectors and returns the result.
    #[Swag.Overload]
    mtd(op: string) opBinary(other: Self)->Self
    {
        #if op == "+":
            return Self{x + other.x, y + other.y, z + other.z}
        #elif op == "-":
            return Self{x - other.x, y - other.y, z - other.z}
        #elif op == "*":
            return Self{x * other.x, y * other.y, z * other.z}
        #elif op == "/":
            return Self{x / other.x, y / other.y, z / other.z}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-scalar operations (+, -, *, /).
    // Applies the scalar 'value' to all components and returns the result.
    #[Swag.Overload]
    mtd(op: string) opBinary(value: f32)->Self
    {
        #if op == "+":
            return Self{x + value, y + value, z + value}
        #elif op == "-":
            return Self{x - value, y - value, z - value}
        #elif op == "*":
            return Self{x * value, y * value, z * value}
        #elif op == "/":
            return Self{x / value, y / value, z / value}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with all components negated.
    mtd(op: string) opUnary()->Self
    {
        #if op == "-":
            return Self{-x, -y, -z}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set all three components of the vector.
    // Parameters 'x', 'y', and 'z' specify the new component values.
    #[Swag.Inline]
    mtd set(x, y, z: f32)
    {
        self.x = #self.x
        self.y = #self.y
        self.z = #self.z
    }

    // Returns the length (magnitude) of the vector.
    // Calculated using the Euclidean distance formula: sqrt(x² + y² + z²).
    mtd const length()->f32 => sqrt((x * x) + (y * y) + (z * z))

    // Returns the squared length of the vector.
    // Faster than 'length()' when only comparing magnitudes since it avoids the square root operation.
    mtd const lengthSquared()->f32 => (x * x) + (y * y) + (z * z)

    // Returns the dot product between this vector and 'other'.
    // The dot product is calculated as: (x₁ × x₂) + (y₁ × y₂) + (z₁ × z₂).
    mtd const dot(other: Self)->f32 => (x * other.x) + (y * other.y) + (z * other.z)

    // Set all vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    mtd clear()
    {
        x, y, z = 0
    }

    // Negate the vector in-place.
    // Multiplies all components by -1, effectively reversing the vector direction.
    mtd negate()
    {
        x = -x
        y = -y
        z = -z
    }

    // Returns true if this vector is exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    mtd const isZero()->bool
    {
        return x == 0 and y == 0 and z == 0
    }

    // Returns true if this vector is zero within an epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps) and isZeroEpsilon(z, eps)
    }

    // Returns true if this vector is equal to 'other' within an epsilon tolerance.
    // Compares each component separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Self, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps) and Math.isEqualEpsilon(z, other.z, eps)
    }

    // Returns the Euclidean distance between this vector and 'dest'.
    // Calculated as the length of the difference vector between the two positions.
    mtd const distance(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        let zn = (z - dest.z)
        return sqrt((xn * xn) + (yn * yn) + (zn * zn))
    }

    // Returns the squared distance between this vector and 'dest'.
    // Faster than 'distance()' when only comparing distances since it avoids the square root operation.
    mtd const distanceSquared(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        let zn = (z - dest.z)
        return (xn * xn) + (yn * yn) + (zn * zn)
    }

    // Normalize this vector in-place to unit length.
    // Sets the vector's length to 1 while preserving its direction. Asserts that the vector is not zero.
    mtd normalize()
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
        z *= norm
    }

    // Returns this vector normalized to unit length without modifying the original.
    // Creates a new vector with length 1 in the same direction. Asserts that the vector is not zero.
    mtd const toNormalize()->Self
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Self{x * norm, y * norm, z * norm}
    }

    // Set the vector to the specified length safely.
    // First normalizes the vector safely, then scales it to 'newLength'. Handles zero-length vectors.
    mtd setLengthSafe(newLength: f32)
    {
        normalizeSafe()
        x *= newLength
        y *= newLength
        z *= newLength
    }

    // Set the vector to the specified length.
    // First normalizes the vector, then scales it to 'newLength'. Assumes the vector is not zero.
    mtd setLength(newLength: f32)
    {
        normalize()
        x *= newLength
        y *= newLength
        z *= newLength
    }

    // Normalize this vector in-place safely.
    // Sets the vector to unit length, or to zero if the original length is near zero.
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

    // Returns this vector normalized safely without modifying the original.
    // Returns a unit vector in the same direction, or the zero vector if length is near zero.
    mtd const toNormalizeSafe()->Self
    {
        var norm = sqrt((x * x) + (y * y) + (z * z))
        if Math.isZeroEpsilon(norm):
            return Zero
        norm = 1 / norm
        return Self{x * norm, y * norm, z * norm}
    }

    // Returns a vector containing the minimum components of two vectors.
    // Each component of the result is the smaller of the corresponding components in 'a' and 'b'.
    func min(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        result.z = Math.min(a.z, b.z)
        return result
    }

    // Returns a vector containing the maximum components of two vectors.
    // Each component of the result is the larger of the corresponding components in 'a' and 'b'.
    func max(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        result.z = Math.max(a.z, b.z)
        return result
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    func lerp(src, dst: Vector3, factor: f32)->Vector3
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        result.z = Math.lerp(src.z, dst.z, factor)
        return result
    }
}
