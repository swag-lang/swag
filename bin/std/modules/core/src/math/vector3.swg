#global namespace Math
#global public

// A 3D vector struct representing a point or direction in 3D space.
// Contains x, y, and z components as 32-bit floating point values.
#[Serialization.Final]
struct Vector3
{
    x, y, z: f32 // X, Y, and Z components of the vector
}

impl Vector3
{
    // Common vector constants
    const Zero:     Vector3{0, 0, 0}                                         // Zero vector (0, 0, 0)
    const One:      Vector3{1, 1, 1}                                         // Unit vector in all directions (1, 1, 1)
    const UnitX:    Vector3{1, 0, 0}                                         // Unit vector along X-axis (1, 0, 0)
    const UnitY:    Vector3{0, 1, 0}                                         // Unit vector along Y-axis (0, 1, 0)
    const UnitZ:    Vector3{0, 0, 1}                                         // Unit vector along Z-axis (0, 0, 1)
    const NegUnitX: Vector3{-1, 0, 0}                                        // Negative unit vector along X-axis (-1, 0, 0)
    const NegUnitY: Vector3{0, -1, 0}                                        // Negative unit vector along Y-axis (0, -1, 0)
    const NegUnitZ: Vector3{0, 0, -1}                                        // Negative unit vector along Z-axis (0, 0, -1)
    const Max:      Vector3{Swag.F32.Max, Swag.F32.Max, Swag.F32.Max}        // Maximum possible vector values
    const NegMax:   Vector3{-Swag.F32.Max, -Swag.F32.Max, -Swag.F32.Max}     // Negative maximum vector values
    const Min:      Vector3{Swag.F32.Min, Swag.F32.Min, Swag.F32.Min}        // Minimum possible vector values

    // Set all components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set all components to 5.0.
    #[Swag.Complete, Swag.Inline]
    mtd opAffect(value: f32)
    {
        me.x, me.y, me.z = value
    }

    // Compound assignment operators for vector-vector operations (+=, -=, *=, /=).
    // Performs component-wise operations with another vector.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(other: Vector3)
    {
        #if op == "+="
        {
            me.x += other.x
            me.y += other.y
            me.z += other.z
        }
        #elif op == "-="
        {
            me.x -= other.x
            me.y -= other.y
            me.z -= other.z
        }
        #elif op == "*="
        {
            me.x *= other.x
            me.y *= other.y
            me.z *= other.z
        }
        #elif op == "/="
        {
            me.x /= other.x
            me.y /= other.y
            me.z /= other.z
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Compound assignment operators for vector-scalar operations (+=, -=, *=, /=).
    // Applies the scalar 'value' to all components of the vector.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(value: f32)
    {
        #if op == "+="
        {
            me.x += value
            me.y += value
            me.z += value
        }
        #elif op == "-="
        {
            me.x -= value
            me.y -= value
            me.z -= value
        }
        #elif op == "*="
        {
            me.x *= value
            me.y *= value
            me.z *= value
        }
        #elif op == "/="
        {
            me.x /= value
            me.y /= value
            me.z /= value
        }
        #else:
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-vector operations (+, -, *, /).
    // Performs component-wise operations between two vectors and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(other: Vector3)->Vector3
    {
        #if op == "+":
            return Vector3{me.x + other.x, me.y + other.y, me.z + other.z}
        #elif op == "-":
            return Vector3{me.x - other.x, me.y - other.y, me.z - other.z}
        #elif op == "*":
            return Vector3{me.x * other.x, me.y * other.y, me.z * other.z}
        #elif op == "/":
            return Vector3{me.x / other.x, me.y / other.y, me.z / other.z}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-scalar operations (+, -, *, /).
    // Applies the scalar 'value' to all components and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(value: f32)->Vector3
    {
        #if op == "+":
            return Vector3{me.x + value, me.y + value, me.z + value}
        #elif op == "-":
            return Vector3{me.x - value, me.y - value, me.z - value}
        #elif op == "*":
            return Vector3{me.x * value, me.y * value, me.z * value}
        #elif op == "/":
            return Vector3{me.x / value, me.y / value, me.z / value}
        #else:
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with all components negated.
    #[Swag.Inline]
    mtd(op: string) const opUnary()->Vector3
    {
        #if op == "-":
            return Vector3{-me.x, -me.y, -me.z}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set all three components of the vector.
    // Parameters 'x', 'y', and 'z' specify the new component values.
    #[Swag.Inline]
    mtd set(x, y, z: f32)
    {
        me.x = x
        me.y = y
        me.z = z
    }

    // Returns the length (magnitude) of the vector.
    // Calculated using the Euclidean distance formula: sqrt(x² + y² + z²).
    #[Swag.Inline]
    mtd const length()->f32 => sqrt((me.x * me.x) + (me.y * me.y) + (me.z * me.z))

    // Returns the squared length of the vector.
    // Faster than 'length()' when only comparing magnitudes since it avoids the square root operation.
    #[Swag.Inline]
    mtd const lengthSquared()->f32 => (me.x * me.x) + (me.y * me.y) + (me.z * me.z)

    // Returns the dot product between this vector and 'other'.
    // The dot product is calculated as: (x₁ × x₂) + (y₁ × y₂) + (z₁ × z₂).
    #[Swag.Inline]
    mtd const dot(other: Vector3)->f32 => (me.x * other.x) + (me.y * other.y) + (me.z * other.z)

    // Returns the cross product between this vector and 'other'.
    // The cross product produces a vector perpendicular to both input vectors.
    #[Swag.Inline]
    mtd const cross(other: Vector3)->Vector3
    {
        return Vector3{(me.y * other.z) - (me.z * other.y), (me.z * other.x) - (me.x * other.z), (me.x * other.y) - (me.y * other.x)}
    }

    // Set all vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    #[Swag.Inline]
    mtd setZero()
    {
        me.x, me.y, me.z = 0
    }

    // Negate the vector in-place.
    // Multiplies all components by -1, effectively reversing the vector direction.
    #[Swag.Inline]
    mtd negate()
    {
        me.x = -me.x
        me.y = -me.y
        me.z = -me.z
    }

    // Returns true if this vector is exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    #[Swag.Inline]
    mtd const isZero()->bool
    {
        return me.x == 0 and me.y == 0 and me.z == 0
    }

    // Returns true if this vector is zero within an epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isZeroEpsilon(me.x, eps) and Math.isZeroEpsilon(me.y, eps) and Math.isZeroEpsilon(me.z, eps)
    }

    // Returns true if this vector is equal to 'other' within an epsilon tolerance.
    // Compares each component separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Vector3, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(me.x, other.x, eps) and Math.isEqualEpsilon(me.y, other.y, eps) and Math.isEqualEpsilon(me.z, other.z, eps)
    }

    // Returns the Euclidean distance between this vector and 'dest'.
    // Calculated as the length of the difference vector between the two positions.
    mtd const distanceTo(dest: Vector3)->f32
    {
        let xn = (me.x - dest.x)
        let yn = (me.y - dest.y)
        let zn = (me.z - dest.z)
        return sqrt((xn * xn) + (yn * yn) + (zn * zn))
    }

    // Returns the squared distance between this vector and 'dest'.
    // Faster than 'distance()' when only comparing distances since it avoids the square root operation.
    mtd const distanceToSquared(dest: Vector3)->f32
    {
        let xn = (me.x - dest.x)
        let yn = (me.y - dest.y)
        let zn = (me.z - dest.z)
        return (xn * xn) + (yn * yn) + (zn * zn)
    }

    // Normalize this vector in-place to unit length.
    // Sets the vector's length to 1 while preserving its direction. Asserts that the vector is not zero.
    mtd normalize()
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y) + (me.z * me.z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        me.x *= norm
        me.y *= norm
        me.z *= norm
    }

    // Returns this vector normalized to unit length without modifying the original.
    // Creates a new vector with length 1 in the same direction. Asserts that the vector is not zero.
    mtd const toNormalized()->Vector3
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y) + (me.z * me.z))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Vector3{me.x * norm, me.y * norm, me.z * norm}
    }

    // Set the vector to the specified length safely.
    // First normalizes the vector safely, then scales it to 'newLength'. Handles zero-length vectors.
    mtd setLengthSafe(newLength: f32)
    {
        me.normalizeSafe()
        me.x *= newLength
        me.y *= newLength
        me.z *= newLength
    }

    // Set the vector to the specified length.
    // First normalizes the vector, then scales it to 'newLength'. Assumes the vector is not zero.
    mtd setLength(newLength: f32)
    {
        me.normalize()
        me.x *= newLength
        me.y *= newLength
        me.z *= newLength
    }

    // Normalize this vector in-place safely.
    // Sets the vector to unit length, or to zero if the original length is near zero.
    mtd normalizeSafe()
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y) + (me.z * me.z))
        if Math.isZeroEpsilon(norm)
        {
            me.x, me.y, me.z = 0
            return
        }

        norm = 1 / norm
        me.x *= norm
        me.y *= norm
        me.z *= norm
    }

    // Returns this vector normalized safely without modifying the original.
    // Returns a unit vector in the same direction, or the zero vector if length is near zero.
    mtd const toNormalizedSafe()->Vector3
    {
        var norm = sqrt((me.x * me.x) + (me.y * me.y) + (me.z * me.z))
        if Math.isZeroEpsilon(norm):
            return Zero
        norm = 1 / norm
        return Vector3{me.x * norm, me.y * norm, me.z * norm}
    }

    // Returns a vector containing the minimum components of two vectors.
    // Each component of the result is the smaller of the corresponding components in 'a' and 'b'.
    func min(a, b: Vector3)->Vector3
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        result.z = Math.min(a.z, b.z)
        return result
    }

    // Returns a vector containing the maximum components of two vectors.
    // Each component of the result is the larger of the corresponding components in 'a' and 'b'.
    func max(a, b: Vector3)->Vector3
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        result.z = Math.max(a.z, b.z)
        return result
    }

    // Apply floor function to all components in-place.
    // Rounds each component down to the nearest integer value.
    mtd floor()
    {
        me.x = Math.floor(me.x)
        me.y = Math.floor(me.y)
        me.z = Math.floor(me.z)
    }

    // Apply ceiling function to all components in-place.
    // Rounds each component up to the nearest integer value.
    mtd ceil()
    {
        me.x = Math.ceil(me.x)
        me.y = Math.ceil(me.y)
        me.z = Math.ceil(me.z)
    }

    // Apply rounding function to all components in-place.
    // Rounds each component to the nearest integer value.
    mtd round()
    {
        me.x = Math.round(me.x)
        me.y = Math.round(me.y)
        me.z = Math.round(me.z)
    }

    // Returns the cosine of the angle between this vector and 'other'.
    // Calculated using the dot product formula: dot(a,b) / (|a| * |b|).
    mtd const cosAngleTo(other: Vector3)->f32
    {
        return me.dot(other) / (me.length() * other.length())
    }

    // Returns the angle between this vector and 'other' in radians.
    // The result is in the range [0, π] using the arc cosine of the normalized dot product.
    mtd const angleTo(other: Vector3)->f32
    {
        return Math.acos(me.cosAngleTo(other))
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    func lerpTo(src, dst: Vector3, factor: f32)->Vector3
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        result.z = Math.lerp(src.z, dst.z, factor)
        return result
    }

    // Clamp this vector's components between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd clamp(min, max: Vector3)
    {
        me.x = Math.clamp(me.x, min.x, max.x)
        me.y = Math.clamp(me.y, min.y, max.y)
        me.z = Math.clamp(me.z, min.z, max.z)
    }

    // Returns a new vector with components clamped between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd const toClamped(min, max: Vector3)->Vector3
    {
        return Vector3{Math.clamp(me.x, min.x, max.x), Math.clamp(me.y, min.y, max.y), Math.clamp(me.z, min.z, max.z)}
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
    mtd const clampedLength(minLength, maxLength: f32)->Vector3
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
    mtd const toAbs()->Vector3
    {
        return Vector3{Math.abs(me.x), Math.abs(me.y), Math.abs(me.z)}
    }

    // Apply absolute value to all components in-place.
    // Makes all components positive by removing their sign.
    mtd makeAbs()
    {
        me.x = Math.abs(me.x)
        me.y = Math.abs(me.y)
        me.z = Math.abs(me.z)
    }

    // Project this vector onto 'other' vector.
    // Returns the component of this vector in the direction of 'other'.
    mtd const toProject(other: Vector3)->Vector3
    {
        let dotProduct    = me.dot(other)
        let otherLengthSq = other.lengthSquared()
        if Math.isZeroEpsilon(otherLengthSq):
            return Zero
        return other * (dotProduct / otherLengthSq)
    }

    // Reflect this vector across a surface with the given 'normal'.
    // The 'normal' should be a unit vector pointing away from the surface.
    mtd const toReflect(normal: Vector3)->Vector3
    {
        return dref me - (normal * (2 * me.dot(normal)))
    }

    // Returns the component-wise sign of the vector (-1, 0, or 1 for each component).
    // Each component becomes -1 if negative, 0 if zero, or 1 if positive.
    mtd const toSign()->Vector3
    {
        return Vector3{Math.sign(me.x), Math.sign(me.y), Math.sign(me.z)}
    }

    // Returns true if all components are finite (not infinite or NaN).
    // Useful for validating vector values after calculations.
    mtd const isFinite()->bool
    {
        return Math.isFinite(me.x) and Math.isFinite(me.y) and Math.isFinite(me.z)
    }

    // Returns true if any component is NaN (Not a Number).
    // Useful for detecting invalid vector values after calculations.
    mtd const isNaN()->bool
    {
        return Math.isNaN(me.x) or Math.isNaN(me.y) or Math.isNaN(me.z)
    }

    // Move this vector towards 'target' by a maximum distance of 'maxDistance'.
    // If the distance to 'target' is less than 'maxDistance', moves directly to target.
    mtd moveTowards(target: Vector3, maxDistance: f32)
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
    mtd const toMovedTowards(target: Vector3, maxDistance: f32)->Vector3
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
    func smoothDamp(current, target: Vector3, currentVelocity: *Vector3, smoothTime, deltaTime: f32)->Vector3
    {
        var result: retval = undefined
        result.x = Math.smoothDamp(current.x, target.x, &currentVelocity.x, smoothTime, deltaTime)
        result.y = Math.smoothDamp(current.y, target.y, &currentVelocity.y, smoothTime, deltaTime)
        result.z = Math.smoothDamp(current.z, target.z, &currentVelocity.z, smoothTime, deltaTime)
        return result
    }

    // Create a vector from spherical coordinates.
    // 'theta' is azimuthal angle in radians, 'phi' is polar angle in radians, 'radius' is the distance from origin.
    func fromSpherical(theta, phi, radius: f32)->Vector3
    {
        let sinPhi = Math.sin(phi)
        return Vector3{radius * sinPhi * Math.cos(theta), radius * Math.cos(phi), radius * sinPhi * Math.sin(theta)}
    }

    // Convert this vector to spherical coordinates.
    // Returns theta (azimuthal angle), phi (polar angle) in radians and radius (magnitude) as a tuple-like struct.
    mtd const toSpherical()->{ theta: f32, phi: f32, radius: f32 }
    {
        let radius = me.length()
        let phi    = Math.acos(me.y / radius)
        let theta  = Math.atan2(me.z, me.x)
        return {theta, phi, radius}
    }

    // Create a vector from cylindrical coordinates.
    // 'theta' is the angle in radians, 'radius' is the distance from the z-axis, 'height' is the z-component.
    func fromCylindrical(theta, radius, height: f32)->Vector3
    {
        return Vector3{radius * Math.cos(theta), height, radius * Math.sin(theta)}
    }

    // Convert this vector to cylindrical coordinates.
    // Returns theta (angle in radians), radius (distance from z-axis) and height (z-component) as a tuple-like struct.
    mtd const toCylindrical()->{ theta: f32, radius: f32, height: f32 }
    {
        let radius = Math.sqrt((me.x * me.x) + (me.z * me.z))
        let theta  = Math.atan2(me.z, me.x)
        return {theta, radius, me.y}
    }
}
