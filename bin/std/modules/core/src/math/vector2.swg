#global marked
#global namespace Math
#global public

// A 2D vector struct representing a point or direction in 2D space.
// Contains x and y components as 32-bit floating point values.
#[Serialization.Final]
struct Vector2
{
    x, y: f32 // X and Y components of the vector
}

impl Vector2
{
    // Common vector constants
    const Zero:     Vector2{0, 0}                             // Zero vector (0, 0)
    const One:      Vector2{1, 1}                             // Unit vector in both directions (1, 1)
    const UnitX:    Vector2{1, 0}                             // Unit vector along X-axis (1, 0)
    const UnitY:    Vector2{0, 1}                             // Unit vector along Y-axis (0, 1)
    const NegUnitX: Vector2{-1, 0}                            // Negative unit vector along X-axis (-1, 0)
    const NegUnitY: Vector2{0, -1}                            // Negative unit vector along Y-axis (0, -1)
    const Max:      Vector2{Swag.F32.Max, Swag.F32.Max}       // Maximum possible vector values
    const NegMax:   Vector2{-Swag.F32.Max, -Swag.F32.Max}     // Negative maximum vector values
    const Min:      Vector2{Swag.F32.Min, Swag.F32.Min}       // Minimum possible vector values

    // Set both components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set both components to 5.0.
    #[Swag.Complete, Swag.Inline]
    mtd opAffect(value: f32)
    {
        me.x, me.y = value
    }

    // Compound assignment operators for vector-vector operations (+=, -=, *=, /=).
    // Performs component-wise operations with another vector.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(other: Vector2)
    {
        #if op == "+="
        {
            me.x += other.x
            me.y += other.y
        }
        #elif op == "-="
        {
            me.x -= other.x
            me.y -= other.y
        }
        #elif op == "*="
        {
            me.x *= other.x
            me.y *= other.y
        }
        #elif op == "/="
        {
            me.x /= other.x
            me.y /= other.y
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
            me.x += value
            me.y += value
        }
        #elif op == "-="
        {
            me.x -= value
            me.y -= value
        }
        #elif op == "*="
        {
            me.x *= value
            me.y *= value
        }
        #elif op == "/="
        {
            me.x /= value
            me.y /= value
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-vector operations (+, -, *, /).
    // Performs component-wise operations between two vectors and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(other: Vector2)->Vector2
    {
        #if op == "+":
            return Vector2{me.x + other.x, me.y + other.y}
        #elif op == "-":
            return Vector2{me.x - other.x, me.y - other.y}
        #elif op == "*":
            return Vector2{me.x * other.x, me.y * other.y}
        #elif op == "/":
            return Vector2{me.x / other.x, me.y / other.y}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-scalar operations (+, -, *, /).
    // Applies the scalar 'value' to both components and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(value: f32)->Vector2
    {
        #if op == "+":
            return Vector2{me.x + value, me.y + value}
        #elif op == "-":
            return Vector2{me.x - value, me.y - value}
        #elif op == "*":
            return Vector2{me.x * value, me.y * value}
        #elif op == "/":
            return Vector2{me.x / value, me.y / value}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with both components negated.
    #[Swag.Inline]
    mtd(op: string) const opUnary()->Vector2
    {
        #if op == "-":
            return Vector2{-me.x, -me.y}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set both components of the vector.
    // Parameters 'x' and 'y' specify the new component values.
    #[Swag.Inline]
    mtd set(x, y: f32)
    {
        me.x = x
        me.y = y
    }

    // Returns the length (magnitude) of the vector.
    // Calculated using the Euclidean distance formula: sqrt(x² + y²).
    #[Swag.Inline]
    mtd const length()->f32 => sqrt((me.x * me.x) + (me.y * me.y))

    // Returns the squared length of the vector.
    // Faster than 'length()' when only comparing magnitudes since it avoids the square root operation.
    #[Swag.Inline]
    mtd const lengthSquared()->f32 => (me.x * me.x) + (me.y * me.y)

    // Returns the dot product between this vector and 'other'.
    // The dot product is calculated as: (x₁ × x₂) + (y₁ × y₂).
    #[Swag.Inline]
    mtd const dot(other: Vector2)->f32 => (me.x * other.x) + (me.y * other.y)

    // Returns the perpendicular vector in the 2D plane scaled by 'axis'.
    // The 'axis' parameter determines the sign and scale of the result.
    #[Swag.Overload, Swag.Inline]
    mtd const cross(axis: f32)->Vector2
    {
        return {me.y * axis, -me.x * axis}
    }

    // Returns the Z component of the cross product (scalar cross product in 2D).
    // Calculates the determinant of the 2x2 matrix formed by the two vectors.
    #[Swag.Overload, Swag.Inline]
    mtd const cross(other: Vector2)->f32
    {
        return me.x * other.y - me.y * other.x
    }

    // Set both vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    #[Swag.Inline]
    mtd setZero()
    {
        me.x, me.y = 0
    }

    // Negate the vector in-place.
    // Multiplies both components by -1, effectively reversing the vector direction.
    #[Swag.Inline]
    mtd negate()
    {
        me.x = -me.x
        me.y = -me.y
    }

    // Returns true if this vector is exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    #[Swag.Inline]
    mtd const isZero()->bool
    {
        return me.x == 0 and me.y == 0
    }

    // Returns true if this vector is zero within an epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isZeroEpsilon(me.x, eps) and Math.isZeroEpsilon(me.y, eps)
    }

    // Returns true if this vector is equal to 'other' within an epsilon tolerance.
    // Compares each component separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Vector2, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(me.x, other.x, eps) and Math.isEqualEpsilon(me.y, other.y, eps)
    }

    // Returns the Euclidean distance between this vector and 'dest'.
    // Calculated as the length of the difference vector between the two positions.
    mtd const distanceTo(dest: Vector2)->f32
    {
        let xn = (me.x - dest.x)
        let yn = (me.y - dest.y)
        return sqrt((xn * xn) + (yn * yn))
    }

    // Returns the squared distance between this vector and 'dest'.
    // Faster than 'distance()' when only comparing distances since it avoids the square root operation.
    mtd const distanceToSquared(dest: Vector2)->f32
    {
        let xn = (me.x - dest.x)
        let yn = (me.y - dest.y)
        return (xn * xn) + (yn * yn)
    }

    // Normalize this vector in-place to unit length.
    // Sets the vector's length to 1 while preserving its direction. Asserts that the vector is not zero.
    mtd normalize()
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        me.x *= norm
        me.y *= norm
    }

    // Returns this vector normalized to unit length without modifying the original.
    // Creates a new vector with length 1 in the same direction. Asserts that the vector is not zero.
    mtd const toNormalized()->Vector2
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Vector2{me.x * norm, me.y * norm}
    }

    // Set the vector to the specified length safely.
    // First normalizes the vector safely, then scales it to 'newLength'. Handles zero-length vectors.
    mtd setLengthSafe(newLength: f32)
    {
        me.normalizeSafe()
        me.x *= newLength
        me.y *= newLength
    }

    // Set the vector to the specified length.
    // First normalizes the vector, then scales it to 'newLength'. Assumes the vector is not zero.
    mtd setLength(newLength: f32)
    {
        me.normalize()
        me.x *= newLength
        me.y *= newLength
    }

    // Normalize this vector in-place safely.
    // Sets the vector to unit length, or to zero if the original length is near zero.
    mtd normalizeSafe()
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y))
        if Math.isZeroEpsilon(norm)
        {
            me.x, me.y = 0
            return
        }

        norm = 1 / norm
        me.x *= norm
        me.y *= norm
    }

    // Returns this vector normalized safely without modifying the original.
    // Returns a unit vector in the same direction, or the zero vector if length is near zero.
    mtd const toNormalizedSafe()->Vector2
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y))
        if Math.isZeroEpsilon(norm):
            return Zero
        norm = 1 / norm
        return Vector2{me.x * norm, me.y * norm}
    }

    // Returns a vector containing the minimum components of two vectors.
    // Each component of the result is the smaller of the corresponding components in 'a' and 'b'.
    func min(a, b: Vector2)->Vector2
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        return result
    }

    // Returns a vector containing the maximum components of two vectors.
    // Each component of the result is the larger of the corresponding components in 'a' and 'b'.
    func max(a, b: Vector2)->Vector2
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
        me.x = Math.floor(me.x)
        me.y = Math.floor(me.y)
    }

    // Apply ceiling function to both components in-place.
    // Rounds each component up to the nearest integer value.
    mtd ceil()
    {
        me.x = Math.ceil(me.x)
        me.y = Math.ceil(me.y)
    }

    // Apply rounding function to both components in-place.
    // Rounds each component to the nearest integer value.
    mtd round()
    {
        me.x = Math.round(me.x)
        me.y = Math.round(me.y)
    }

    // Rotate the vector by a given angle around the origin.
    // Uses standard 2D rotation matrix transformation with the specified 'angle'.
    mtd rotateBy(angle: Math.Angle)
    {
        let s  = Math.sin(angle.rad)
        let c  = Math.cos(angle.rad)
        let tx = me.x
        let ty = me.y
        me.x = (c * tx) - (s * ty)
        me.y = (s * tx) + (c * ty)
    }

    // Returns the cosine of the angle between this vector and 'other'.
    // Calculated using the dot product formula: dot(a,b) / (|a| * |b|).
    mtd const cosAngleTo(other: Vector2)->f32
    {
        return me.dot(other) / (me.length() * other.length())
    }

    // Returns the angle between this vector and 'other' in radians.
    // The result is in the range [0, π] using the arc cosine of the normalized dot product.
    mtd const angleTo(other: Vector2)->f32
    {
        return Math.acos(me.cosAngleTo(other))
    }

    // Returns the angle between this vector and 'other' in the range [0, 2π].
    // Uses the cross product to determine the sign and adjust the angle accordingly.
    mtd const angle0To2Pi(other: Vector2)->f32
    {
        var angle = Math.acos(me.cosAngleTo(other))
        let sign  = (me.x * other.y) - (me.y * other.x)
        if sign > 0:
            angle = ConstF32.TwoPi - angle
        return angle
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    mtd lerpTo(dst: Vector2, factor: f32)->Vector2
    {
        var result: retval = undefined
        result.x = Math.lerp(me.x, dst.x, factor)
        result.y = Math.lerp(me.y, dst.y, factor)
        return result
    }

    // Clamp this vector's components between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd clamp(min, max: Vector2)
    {
        me.x = Math.clamp(me.x, min.x, max.x)
        me.y = Math.clamp(me.y, min.y, max.y)
    }

    // Returns a new vector with components clamped between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd const toClamped(min, max: Vector2)->Vector2
    {
        return Vector2{Math.clamp(me.x, min.x, max.x), Math.clamp(me.y, min.y, max.y)}
    }

    // Clamp this vector's length to be within 'minLength' and 'maxLength'.
    // Preserves the vector's direction while constraining its magnitude.
    mtd clampLength(minLength, maxLength: f32)
    {
        let len = me.length()
        if len < minLength:
            me.setLength(minLength)
        elif len > maxLength:
            me.setLength(maxLength)
    }

    // Returns a new vector with length clamped between 'minLength' and 'maxLength'.
    // Preserves the vector's direction while constraining its magnitude.
    mtd const clampedLength(minLength, maxLength: f32)->Vector2
    {
        let len = me.length()
        if len < minLength:
            return me.toNormalized() * minLength
        elif len > maxLength:
            return me.toNormalized() * maxLength
        return dref me
    }

    // Returns the absolute value of each component.
    // Creates a new vector where each component is its absolute value.
    mtd const toAbs()->Vector2
    {
        return Vector2{Math.abs(me.x), Math.abs(me.y)}
    }

    // Apply absolute value to both components in-place.
    // Makes both components positive by removing their sign.
    mtd makeAbs()
    {
        me.x = Math.abs(me.x)
        me.y = Math.abs(me.y)
    }

    // Returns a vector perpendicular to this one (rotated 90 degrees counter-clockwise).
    // The perpendicular vector has the same length but is rotated 90° CCW.
    mtd const toPerp()->Vector2
    {
        return Vector2{-me.y, me.x}
    }

    // Returns a vector perpendicular to this one (rotated 90 degrees clockwise).
    // The perpendicular vector has the same length but is rotated 90° CW.
    mtd const toPerpCW()->Vector2
    {
        return Vector2{me.y, -me.x}
    }

    // Project this vector onto 'other' vector.
    // Returns the component of this vector in the direction of 'other'.
    mtd const toProject(other: Vector2)->Vector2
    {
        let dotProduct    = me.dot(other)
        let otherLengthSq = other.lengthSquared()
        if Math.isZeroEpsilon(otherLengthSq):
            return Zero
        return other * (dotProduct / otherLengthSq)
    }

    // Reflect this vector across a surface with the given 'normal'.
    // The 'normal' should be a unit vector pointing away from the surface.
    mtd const toReflect(normal: Vector2)->Vector2
    {
        return dref me - (normal * (2 * me.dot(normal)))
    }

    // Returns the component-wise sign of the vector (-1, 0, or 1 for each component).
    // Each component becomes -1 if negative, 0 if zero, or 1 if positive.
    mtd const toSign()->Vector2
    {
        return Vector2{Math.sign(me.x), Math.sign(me.y)}
    }

    // Returns true if all components are finite (not infinite or NaN).
    // Useful for validating vector values after calculations.
    mtd const isFinite()->bool
    {
        return Math.isFinite(me.x) and Math.isFinite(me.y)
    }

    // Returns true if any component is NaN (Not a Number).
    // Useful for detecting invalid vector values after calculations.
    mtd const isNaN()->bool
    {
        return Math.isNaN(me.x) or Math.isNaN(me.y)
    }

    // Move this vector towards 'target' by a maximum distance of 'maxDistance'.
    // If the distance to 'target' is less than 'maxDistance', moves directly to target.
    mtd moveTowards(target: Vector2, maxDistance: f32)
    {
        let dir  = target - dref me
        let dist = dir.length()
        if dist <= maxDistance:
            dref me = target
        else:
            dref me += dir * (maxDistance / dist)
    }

    // Returns a vector moved towards 'target' by a maximum distance of 'maxDistance'.
    // If the distance to 'target' is less than 'maxDistance', returns the target directly.
    mtd const toMovedTowards(target: Vector2, maxDistance: f32)->Vector2
    {
        let dir  = target - dref me
        let dist = dir.length()
        if dist <= maxDistance:
            return target
        return dref me + (dir * (maxDistance / dist))
    }

    // Perform smooth damping interpolation towards 'target'.
    // Uses 'smoothTime' and 'deltaTime' for frame-rate independent smooth movement.
    // 'currentVelocity' is modified to track the current rate of change.
    func smoothDamp(current, target: Vector2, currentVelocity: &Vector2, smoothTime, deltaTime: f32)->Vector2
    {
        var result: retval = undefined
        result.x = Math.smoothDamp(current.x, target.x, &currentVelocity.x, smoothTime, deltaTime)
        result.y = Math.smoothDamp(current.y, target.y, &currentVelocity.y, smoothTime, deltaTime)
        return result
    }

    // Create a vector from polar coordinates.
    // 'angle' is in radians, 'radius' is the distance from origin.
    func fromPolar(angle, radius: f32)->Vector2
    {
        return Vector2{radius * cos(angle), radius * sin(angle)}
    }

    // Convert this vector to polar coordinates.
    // Returns angle in radians and radius (magnitude) as a tuple-like struct.
    mtd const toPolar()->{ angle: f32, radius: f32 }
    {
        return {Math.atan2(me.y, me.x), me.length()}
    }

    // Returns the angle of this vector in radians.
    // The angle is measured from the positive X-axis, counter-clockwise.
    mtd const toAngle()->f32
    {
        return Math.atan2(me.y, me.x)
    }

    // Rotate this vector to face the specified 'angle' in radians.
    // Preserves the vector's magnitude while changing its direction.
    mtd setAngle(angle: f32)
    {
        let magnitude = me.length()
        me.x = magnitude * Math.cos(angle)
        me.y = magnitude * Math.sin(angle)
    }
}
