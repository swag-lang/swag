#global namespace Math
#global public

// A 2D vector struct representing a point or direction in 2D space.
// Contains x and y components as 32-bit floating point values.
#[Serialization.Final]
struct Vector2
{
    x, y: f32
     // X and Y components of the vector
}

impl Vector2
{
    // Common vector constants
    const Zero:     Self{0, 0}                             // Zero vector (0, 0)
    const One:      Self{1, 1}                             // Unit vector in both directions (1, 1)
    const UnitX:    Self{1, 0}                             // Unit vector along X-axis (1, 0)
    const UnitY:    Self{0, 1}                             // Unit vector along Y-axis (0, 1)
    const NegUnitX: Self{-1, 0}                            // Negative unit vector along X-axis (-1, 0)
    const NegUnitY: Self{0, -1}                            // Negative unit vector along Y-axis (0, -1)
    const Max:      Self{Swag.F32.Max, Swag.F32.Max}       // Maximum possible vector values
    const NegMax:   Self{-Swag.F32.Max, -Swag.F32.Max}     // Negative maximum vector values
    const Min:      Self{Swag.F32.Min, Swag.F32.Min}       // Minimum possible vector values

    // Set both components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set both components to 5.0.
    #[Swag.Complete, Swag.Inline]
    mtd opAffect(value: f32)
    {
        x, y = value
    }

    // Compound assignment operators for vector-vector operations (+=, -=, *=, /=).
    // Performs component-wise operations with another vector.
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

    // Compound assignment operators for vector-scalar operations (+=, -=, *=, /=).
    // Applies the scalar 'value' to both components of the vector.
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

    // Binary operators for vector-vector operations (+, -, *, /).
    // Performs component-wise operations between two vectors and returns the result.
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

    // Binary operators for vector-scalar operations (+, -, *, /).
    // Applies the scalar 'value' to both components and returns the result.
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

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with both components negated.
    #[Swag.Inline]
    mtd(op: string) const opUnary()->Self
    {
        #if op == "-":
            return Self{-x, -y}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set both components of the vector.
    // Parameters 'x' and 'y' specify the new component values.
    #[Swag.Inline]
    mtd set(x, y: f32)
    {
        self.x = #self.x
        self.y = #self.y
    }

    // Returns the length (magnitude) of the vector.
    // Calculated using the Euclidean distance formula: sqrt(x² + y²).
    #[Swag.Inline]
    mtd const length()->f32 => sqrt((x * x) + (y * y))

    // Returns the squared length of the vector.
    // Faster than 'length()' when only comparing magnitudes since it avoids the square root operation.
    #[Swag.Inline]
    mtd const lengthSquared()->f32 => (x * x) + (y * y)

    // Returns the dot product between this vector and 'other'.
    // The dot product is calculated as: (x₁ × x₂) + (y₁ × y₂).
    #[Swag.Inline]
    mtd const dot(other: Self)->f32 => (x * other.x) + (y * other.y)

    // Returns the perpendicular vector in the 2D plane scaled by 'axis'.
    // The 'axis' parameter determines the sign and scale of the result.
    #[Swag.Overload, Swag.Inline]
    mtd const cross(axis: f32)->Self
    {
        return {y * axis, -x * axis}
    }

    // Returns the Z component of the cross product (scalar cross product in 2D).
    // Calculates the determinant of the 2x2 matrix formed by the two vectors.
    #[Swag.Overload, Swag.Inline]
    mtd const cross(other: Self)->f32
    {
        return x * other.y - y * other.x
    }

    // Set both vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    #[Swag.Inline]
    mtd clear()
    {
        x, y = 0
    }

    // Negate the vector in-place.
    // Multiplies both components by -1, effectively reversing the vector direction.
    #[Swag.Inline]
    mtd negate()
    {
        x = -x
        y = -y
    }

    // Returns true if this vector is exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    #[Swag.Inline]
    mtd const isZero()->bool
    {
        return x == 0 and y == 0
    }

    // Returns true if this vector is zero within an epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return isZeroEpsilon(x, eps) and isZeroEpsilon(y, eps)
    }

    // Returns true if this vector is equal to 'other' within an epsilon tolerance.
    // Compares each component separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Self, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(x, other.x, eps) and Math.isEqualEpsilon(y, other.y, eps)
    }

    // Returns the Euclidean distance between this vector and 'dest'.
    // Calculated as the length of the difference vector between the two positions.
    mtd const distance(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        return sqrt((xn * xn) + (yn * yn))
    }

    // Returns the squared distance between this vector and 'dest'.
    // Faster than 'distance()' when only comparing distances since it avoids the square root operation.
    mtd const distanceSquared(dest: Self)->f32
    {
        let xn = (x - dest.x)
        let yn = (y - dest.y)
        return (xn * xn) + (yn * yn)
    }

    // Normalize this vector in-place to unit length.
    // Sets the vector's length to 1 while preserving its direction. Asserts that the vector is not zero.
    mtd normalize()
    {
        var norm = sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        x *= norm
        y *= norm
    }

    // Returns this vector normalized to unit length without modifying the original.
    // Creates a new vector with length 1 in the same direction. Asserts that the vector is not zero.
    mtd const toNormalize()->Self
    {
        var norm = sqrt((x * x) + (y * y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Self{x * norm, y * norm}
    }

    // Set the vector to the specified length safely.
    // First normalizes the vector safely, then scales it to 'newLength'. Handles zero-length vectors.
    mtd setLengthSafe(newLength: f32)
    {
        normalizeSafe()
        x *= newLength
        y *= newLength
    }

    // Set the vector to the specified length.
    // First normalizes the vector, then scales it to 'newLength'. Assumes the vector is not zero.
    mtd setLength(newLength: f32)
    {
        normalize()
        x *= newLength
        y *= newLength
    }

    // Normalize this vector in-place safely.
    // Sets the vector to unit length, or to zero if the original length is near zero.
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

    // Returns this vector normalized safely without modifying the original.
    // Returns a unit vector in the same direction, or the zero vector if length is near zero.
    mtd const toNormalizeSafe()->Self
    {
        var norm = sqrt((x * x) + (y * y))
        if Math.isZeroEpsilon(norm):
            return Zero
        norm = 1 / norm
        return Self{x * norm, y * norm}
    }

    // Returns a vector containing the minimum components of two vectors.
    // Each component of the result is the smaller of the corresponding components in 'a' and 'b'.
    func min(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        return result
    }

    // Returns a vector containing the maximum components of two vectors.
    // Each component of the result is the larger of the corresponding components in 'a' and 'b'.
    func max(a, b: Self)->Self
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        return result
    }

    // Apply floor function to both components in-place.
    // Rounds each component down to the nearest integer value.
    mtd floor()
    {
        x = Math.floor(x)
        y = Math.floor(y)
    }

    // Apply ceiling function to both components in-place.
    // Rounds each component up to the nearest integer value.
    mtd ceil()
    {
        x = Math.ceil(x)
        y = Math.ceil(y)
    }

    // Apply rounding function to both components in-place.
    // Rounds each component to the nearest integer value.
    mtd round()
    {
        x = Math.round(x)
        y = Math.round(y)
    }

    // Rotate the vector by a given angle around the origin.
    // Uses standard 2D rotation matrix transformation with the specified 'angle'.
    mtd rotate(angle: Math.Angle)
    {
        let s  = sin(angle.rad)
        let c  = cos(angle.rad)
        let tx = x
        let ty = y
        x = (c * tx) - (s * ty)
        y = (s * tx) + (c * ty)
    }

    // Returns the cosine of the angle between this vector and 'other'.
    // Calculated using the dot product formula: dot(a,b) / (|a| * |b|).
    mtd const cosAngle(other: Self)->f32
    {
        return dot(other) / (length() * other.length())
    }

    // Returns the angle between this vector and 'other' in radians.
    // The result is in the range [0, π] using the arc cosine of the normalized dot product.
    mtd const angle(other: Self)->f32
    {
        return Math.acos(cosAngle(other))
    }

    // Returns the angle between this vector and 'other' in the range [0, 2π].
    // Uses the cross product to determine the sign and adjust the angle accordingly.
    mtd const angle0To2Pi(other: Self)->f32
    {
        var angle = Math.acos(cosAngle(other))
        let sign  = (x * other.y) - (y * other.x)
        if sign > 0:
            angle = ConstF32.TwoPi - angle
        return angle
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    func lerp(src, dst: Vector2, factor: f32)->Vector2
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        return result
    }
}
