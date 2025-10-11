#global namespace Math
#global public

// A 4D vector struct representing a point or direction in 4D space.
// Contains x, y, z, and w components as 32-bit floating point values.
// Commonly used for homogeneous coordinates in 3D graphics or RGBA color values.
#[Serialization.Final]
struct Vector4
{
    x, y, z, w: f32 // X, Y, Z, and W components of the vector
}

impl Vector4
{
    // Common vector constants
    const Zero:     Vector4{0, 0, 0, 0}                                                     // Zero vector (0, 0, 0, 0)
    const One:      Vector4{1, 1, 1, 1}                                                     // Unit vector in all directions (1, 1, 1, 1)
    const UnitX:    Vector4{1, 0, 0, 0}                                                     // Unit vector along X-axis (1, 0, 0, 0)
    const UnitY:    Vector4{0, 1, 0, 0}                                                     // Unit vector along Y-axis (0, 1, 0, 0)
    const UnitZ:    Vector4{0, 0, 1, 0}                                                     // Unit vector along Z-axis (0, 0, 1, 0)
    const UnitW:    Vector4{0, 0, 0, 1}                                                     // Unit vector along W-axis (0, 0, 0, 1)
    const NegUnitX: Vector4{-1, 0, 0, 0}                                                    // Negative unit vector along X-axis (-1, 0, 0, 0)
    const NegUnitY: Vector4{0, -1, 0, 0}                                                    // Negative unit vector along Y-axis (0, -1, 0, 0)
    const NegUnitZ: Vector4{0, 0, -1, 0}                                                    // Negative unit vector along Z-axis (0, 0, -1, 0)
    const NegUnitW: Vector4{0, 0, 0, -1}                                                    // Negative unit vector along W-axis (0, 0, 0, -1)
    const Max:      Vector4{Swag.F32.Max, Swag.F32.Max, Swag.F32.Max, Swag.F32.Max}         // Maximum possible vector values
    const NegMax:   Vector4{-Swag.F32.Max, -Swag.F32.Max, -Swag.F32.Max, -Swag.F32.Max}     // Negative maximum vector values
    const Min:      Vector4{Swag.F32.Min, Swag.F32.Min, Swag.F32.Min, Swag.F32.Min}         // Minimum possible vector values

    // Set all components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set all components to 5.0.
    #[Swag.Complete, Swag.Inline]
    mtd opAffect(value: f32)
    {
        .x, .y, .z, .w = value
    }

    // Compound assignment operators for vector-vector operations (+=, -=, *=, /=).
    // Performs component-wise operations with another vector.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(other: Vector4)
    {
        #if op == "+="
        {
            .x += other.x
            .y += other.y
            .z += other.z
            .w += other.w
        }
        #elif op == "-="
        {
            .x -= other.x
            .y -= other.y
            .z -= other.z
            .w -= other.w
        }
        #elif op == "*="
        {
            .x *= other.x
            .y *= other.y
            .z *= other.z
            .w *= other.w
        }
        #elif op == "/="
        {
            .x /= other.x
            .y /= other.y
            .z /= other.z
            .w /= other.w
        }
        #else do
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Compound assignment operators for vector-scalar operations (+=, -=, *=, /=).
    // Applies the scalar 'value' to all components of the vector.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) opAssign(value: f32)
    {
        #if op == "+="
        {
            .x += value
            .y += value
            .z += value
            w += value
        }
        #elif op == "-="
        {
            .x -= value
            .y -= value
            .z -= value
            .w -= value
        }
        #elif op == "*="
        {
            .x *= value
            .y *= value
            .z *= value
            .w *= value
        }
        #elif op == "/="
        {
            .x /= value
            .y /= value
            .z /= value
            .w /= value
        }
        #else do
            #error("unsupported assign operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-vector operations (+, -, *, /).
    // Performs component-wise operations between two vectors and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(other: Vector4)->Vector4
    {
        #if op == "+" do
            return Vector4{.x + other.x, .y + other.y, .z + other.z, .w + other.w}
        #elif op == "-" do
            return Vector4{.x - other.x, .y - other.y, .z - other.z, .w - other.w}
        #elif op == "*" do
            return Vector4{.x * other.x, .y * other.y, .z * other.z, .w * other.w}
        #elif op == "/" do
            return Vector4{.x / other.x, .y / other.y, .z / other.z, .w / other.w}
        #else do
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Binary operators for vector-scalar operations (+, -, *, /).
    // Applies the scalar 'value' to all components and returns the result.
    #[Swag.Overload, Swag.Inline]
    mtd(op: string) const opBinary(value: f32)->Vector4
    {
        #if op == "+" do
            return Vector4{.x + value, .y + value, .z + value, .w + value}
        #elif op == "-" do
            return Vector4{.x - value, .y - value, .z - value, .w - value}
        #elif op == "*" do
            return Vector4{.x * value, .y * value, .z * value, .w * value}
        #elif op == "/" do
            return Vector4{.x / value, .y / value, .z / value, .w / value}
        #else do
            #error("unsupported binary operation [[" ++ op ++ "]]")
    }

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with all components negated.
    #[Swag.Inline]
    mtd(op: string) const opUnary()->Vector4
    {
        #if op == "-" do
            return Vector4{-.x, -.y, -.z, -.w}
        #else do
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set all four components of the vector.
    // Parameters 'x', 'y', 'z', and 'w' specify the new component values.
    #[Swag.Inline]
    mtd set(x, y, z, w: f32)
    {
        .x = x
        .y = y
        .z = z
        .w = w
    }

    // Returns the length (magnitude) of the vector.
    // Calculated using the Euclidean distance formula: sqrt(x² + y² + z² + w²).
    #[Swag.Inline]
    mtd const length()->f32 => sqrt((.x * .x) + (.y * .y) + (.z * .z) + (.w * .w))

    // Returns the squared length of the vector.
    // Faster than 'length()' when only comparing magnitudes since it avoids the square root operation.
    #[Swag.Inline]
    mtd const lengthSquared()->f32 => (.x * .x) + (.y * .y) + (.z * .z) + (.w * .w)

    // Returns the dot product between this vector and 'other'.
    // The dot product is calculated as: (x₁ × x₂) + (y₁ × y₂) + (z₁ × z₂) + (w₁ × w₂).
    #[Swag.Inline]
    mtd const dot(other: Vector4)->f32 => (.x * other.x) + (.y * other.y) + (.z * other.z) + (.w * other.w)

    // Set all vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    #[Swag.Inline]
    mtd setZero()
    {
        .x, .y, .z, .w = 0
    }

    // Negate the vector in-place.
    // Multiplies all components by -1, effectively reversing the vector direction.
    #[Swag.Inline]
    mtd negate()
    {
        .x = -.x
        .y = -.y
        .z = -.z
        .w = -.w
    }

    // Returns true if this vector is exactly zero.
    // Performs exact floating-point comparison, which may not work well with computed values.
    #[Swag.Inline]
    mtd const isZero()->bool
    {
        return .x == 0 and .y == 0 and .z == 0 and .w == 0
    }

    // Returns true if this vector is zero within an epsilon tolerance.
    // Uses 'eps' parameter to handle floating-point precision issues.
    mtd const isZeroEpsilon(eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isZeroEpsilon(.x, eps) and Math.isZeroEpsilon(.y, eps) and Math.isZeroEpsilon(.z, eps) and Math.isZeroEpsilon(.w, eps)
    }

    // Returns true if this vector is equal to 'other' within an epsilon tolerance.
    // Compares each component separately using the specified 'eps' tolerance.
    mtd const isEqualEpsilon(other: Vector4, eps = cast(f32) ConstF64.Epsilon)->bool
    {
        return Math.isEqualEpsilon(.x, other.x, eps) and Math.isEqualEpsilon(.y, other.y, eps) and Math.isEqualEpsilon(.z, other.z, eps) and Math.isEqualEpsilon(.w, other.w, eps)
    }

    // Returns the Euclidean distance between this vector and 'dest'.
    // Calculated as the length of the difference vector between the two positions.
    mtd const distanceTo(dest: Vector4)->f32
    {
        let xn = (.x - dest.x)
        let yn = (.y - dest.y)
        let zn = (.z - dest.z)
        let wn = (.w - dest.w)
        return sqrt((xn * xn) + (yn * yn) + (zn * zn) + (wn * wn))
    }

    // Returns the squared distance between this vector and 'dest'.
    // Faster than 'distance()' when only comparing distances since it avoids the square root operation.
    mtd const distanceToSquared(dest: Vector4)->f32
    {
        let xn = (.x - dest.x)
        let yn = (.y - dest.y)
        let zn = (.z - dest.z)
        let wn = (.w - dest.w)
        return (xn * xn) + (yn * yn) + (zn * zn) + (wn * wn)
    }

    // Normalize this vector in-place to unit length.
    // Sets the vector's length to 1 while preserving its direction. Asserts that the vector is not zero.
    mtd normalize()
    {
        var norm = sqrt((.x * .x) + (.y * .y) + (.z * .z) + (.w * .w))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        .x *= norm
        .y *= norm
        .z *= norm
        .w *= norm
    }

    // Returns this vector normalized to unit length without modifying the original.
    // Creates a new vector with length 1 in the same direction. Asserts that the vector is not zero.
    mtd const toNormalized()->Vector4
    {
        var norm = sqrt((.x * .x) + (.y * .y) + (.z * .z) + (.w * .w))
        Debug.assert(!Math.isZeroEpsilon(norm))
        norm = 1 / norm
        return Vector4{.x * norm, .y * norm, .z * norm, .w * norm}
    }

    // Set the vector to the specified length safely.
    // First normalizes the vector safely, then scales it to 'newLength'. Handles zero-length vectors.
    mtd setLengthSafe(newLength: f32)
    {
        .normalizeSafe()
        .x *= newLength
        .y *= newLength
        .z *= newLength
        .w *= newLength
    }

    // Set the vector to the specified length.
    // First normalizes the vector, then scales it to 'newLength'. Assumes the vector is not zero.
    mtd setLength(newLength: f32)
    {
        .normalize()
        .x *= newLength
        .y *= newLength
        .z *= newLength
        .w *= newLength
    }

    // Normalize this vector in-place safely.
    // Sets the vector to unit length, or to zero if the original length is near zero.
    mtd normalizeSafe()
    {
        var norm = sqrt((.x * .x) + (.y * .y) + (.z * .z) + (.w * .w))
        if Math.isZeroEpsilon(norm)
        {
            .x, .y, .z, .w = 0
            return
        }

        norm = 1 / norm
        .x *= norm
        .y *= norm
        .z *= norm
        .w *= norm
    }

    // Returns this vector normalized safely without modifying the original.
    // Returns a unit vector in the same direction, or the zero vector if length is near zero.
    mtd const toNormalizedSafe()->Vector4
    {
        var norm = sqrt((.x * .x) + (.y * .y) + (.z * .z) + (.w * .w))
        if Math.isZeroEpsilon(norm) do
            return Zero
        norm = 1 / norm
        return Vector4{.x * norm, .y * norm, .z * norm, .w * norm}
    }

    // Returns a vector containing the minimum components of two vectors.
    // Each component of the result is the smaller of the corresponding components in 'a' and 'b'.
    func min(a, b: Vector4)->Vector4
    {
        var result: retval = undefined
        result.x = Math.min(a.x, b.x)
        result.y = Math.min(a.y, b.y)
        result.z = Math.min(a.z, b.z)
        result.w = Math.min(a.w, b.w)
        return result
    }

    // Returns a vector containing the maximum components of two vectors.
    // Each component of the result is the larger of the corresponding components in 'a' and 'b'.
    func max(a, b: Vector4)->Vector4
    {
        var result: retval = undefined
        result.x = Math.max(a.x, b.x)
        result.y = Math.max(a.y, b.y)
        result.z = Math.max(a.z, b.z)
        result.w = Math.max(a.w, b.w)
        return result
    }

    // Apply floor function to all components in-place.
    // Rounds each component down to the nearest integer value.
    mtd floor()
    {
        .x = Math.floor(.x)
        .y = Math.floor(.y)
        .z = Math.floor(.z)
        .w = Math.floor(.w)
    }

    // Apply ceiling function to all components in-place.
    // Rounds each component up to the nearest integer value.
    mtd ceil()
    {
        .x = Math.ceil(.x)
        .y = Math.ceil(.y)
        .z = Math.ceil(.z)
        .w = Math.ceil(.w)
    }

    // Apply rounding function to all components in-place.
    // Rounds each component to the nearest integer value.
    mtd round()
    {
        .x = Math.round(.x)
        .y = Math.round(.y)
        .z = Math.round(.z)
        .w = Math.round(.w)
    }

    // Returns the cosine of the angle between this vector and 'other'.
    // Calculated using the dot product formula: dot(a,b) / (|a| * |b|).
    mtd const cosAngleTo(other: Vector4)->f32
    {
        return .dot(other) / (.length() * other.length())
    }

    // Returns the angle between this vector and 'other' in radians.
    // The result is in the range [0, π] using the arc cosine of the normalized dot product.
    mtd const angleTo(other: Vector4)->f32
    {
        return Math.acos(.cosAngleTo(other))
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    func lerpTo(src, dst: Vector4, factor: f32)->Vector4
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        result.z = Math.lerp(src.z, dst.z, factor)
        result.w = Math.lerp(src.w, dst.w, factor)
        return result
    }

    // Clamp this vector's components between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd clamp(min, max: Vector4)
    {
        .x = Math.clamp(.x, min.x, max.x)
        .y = Math.clamp(.y, min.y, max.y)
        .z = Math.clamp(.z, min.z, max.z)
        .w = Math.clamp(.w, min.w, max.w)
    }

    // Returns a new vector with components clamped between 'min' and 'max' values.
    // Each component is independently clamped to stay within the specified range.
    mtd const toClamped(min, max: Vector4)->Vector4
    {
        return Vector4{Math.clamp(.x, min.x, max.x), Math.clamp(.y, min.y, max.y), Math.clamp(.z, min.z, max.z), Math.clamp(.w, min.w, max.w)}
    }

    // Clamp this vector's length to be within 'minLength' and 'maxLength'.
    // Preserves the vector's direction while constraining its magnitude.
    mtd clampLength(minLength, maxLength: f32)
    {
        let len = .length()
        if len < minLength do
            .setLength(minLength)
        elif len > maxLength do
            .setLength(maxLength)
    }

    // Returns a new vector with length clamped between 'minLength' and 'maxLength'.
    // Preserves the vector's direction while constraining its magnitude.
    mtd const clampedLength(minLength, maxLength: f32)->Vector4
    {
        let len = .length()
        if len < minLength do
            return .toNormalized() * minLength
        elif len > maxLength do
            return .toNormalized() * maxLength
        return dref me
    }

    // Returns the absolute value of each component.
    // Creates a new vector where each component is its absolute value.
    mtd const toAbs()->Vector4
    {
        return Vector4{Math.abs(.x), Math.abs(.y), Math.abs(.z), Math.abs(.w)}
    }

    // Apply absolute value to all components in-place.
    // Makes all components positive by removing their sign.
    mtd makeAbs()
    {
        .x = Math.abs(.x)
        .y = Math.abs(.y)
        .z = Math.abs(.z)
        .w = Math.abs(.w)
    }

    // Project this vector onto 'other' vector.
    // Returns the component of this vector in the direction of 'other'.
    mtd const toProject(other: Vector4)->Vector4
    {
        let dotProduct    = .dot(other)
        let otherLengthSq = other.lengthSquared()
        if Math.isZeroEpsilon(otherLengthSq) do
            return Zero
        return other * (dotProduct / otherLengthSq)
    }

    // Reflect this vector across a surface with the given 'normal'.
    // The 'normal' should be a unit vector pointing away from the surface.
    mtd const toReflect(normal: Vector4)->Vector4
    {
        return dref me - (normal * (2 * .dot(normal)))
    }

    // Returns the component-wise sign of the vector (-1, 0, or 1 for each component).
    // Each component becomes -1 if negative, 0 if zero, or 1 if positive.
    mtd const toSign()->Vector4
    {
        return Vector4{Math.sign(.x), Math.sign(.y), Math.sign(.z), Math.sign(.w)}
    }

    // Returns true if all components are finite (not infinite or NaN).
    // Useful for validating vector values after calculations.
    mtd const isFinite()->bool
    {
        return Math.isFinite(.x) and Math.isFinite(.y) and Math.isFinite(.z) and Math.isFinite(.w)
    }

    // Returns true if any component is NaN (Not a Number).
    // Useful for detecting invalid vector values after calculations.
    mtd const isNaN()->bool
    {
        return Math.isNaN(.x) or Math.isNaN(.y) or Math.isNaN(.z) or Math.isNaN(.w)
    }

    // Move this vector towards 'target' by a maximum distance of 'maxDistance'.
    // If the distance to 'target' is less than 'maxDistance', moves directly to target.
    mtd moveTowards(target: Vector4, maxDistance: f32)
    {
        let dir  = target - dref me
        let dist = dir.length()
        if dist <= maxDistance do
            dref me = target
        else do
            dref me += dir * (maxDistance / dist)
    }

    // Returns a vector moved towards 'target' by a maximum distance of 'maxDistance'.
    // If the distance to 'target' is less than 'maxDistance', returns the target directly.
    mtd const toMovedTowards(target: Vector4, maxDistance: f32)->Vector4
    {
        let dir  = target - dref me
        let dist = dir.length()
        if dist <= maxDistance do
            return target
        return dref me + (dir * (maxDistance / dist))
    }

    // Perform smooth damping interpolation towards 'target'.
    // Uses 'smoothTime' and 'deltaTime' for frame-rate independent smooth movement.
    // 'currentVelocity' is modified to track the current rate of change.
    func smoothDamp(current, target: Vector4, currentVelocity: &Vector4, smoothTime, deltaTime: f32)->Vector4
    {
        var result: retval = undefined
        result.x = Math.smoothDamp(current.x, target.x, &currentVelocity.x, smoothTime, deltaTime)
        result.y = Math.smoothDamp(current.y, target.y, &currentVelocity.y, smoothTime, deltaTime)
        result.z = Math.smoothDamp(current.z, target.z, &currentVelocity.z, smoothTime, deltaTime)
        result.w = Math.smoothDamp(current.w, target.w, &currentVelocity.w, smoothTime, deltaTime)
        return result
    }

    // Convert homogeneous coordinates to 3D by dividing x, y, z by w.
    // Returns a Vector3 representing the 3D position when w != 0. Handles w = 0 case safely.
    mtd const toVector3()->Vector3
    {
        if Math.isZeroEpsilon(.w) do
            return Vector3{.x, .y, .z}
        return Vector3{.x / .w, .y / .w, .z / .w}
    }

    // Extract the XYZ components as a Vector3, ignoring the W component.
    // Useful when Vector4 is used for color (RGBA) and you need just RGB.
    mtd const xyz()->Vector3
    {
        return Vector3{.x, .y, .z}
    }

    // Extract the XY components as a Vector2, ignoring Z and W components.
    // Useful for 2D projections or when using Vector4 for padding.
    mtd const xy()->Vector2
    {
        return Vector2{.x, .y}
    }

    // Create a Vector4 from Vector3 with specified W component.
    // Commonly used for creating homogeneous coordinates from 3D positions.
    func fromVector3(v: Vector3, w: f32)->Vector4
    {
        return Vector4{v.x, v.y, v.z, w}
    }

    // Create a Vector4 from Vector2 with specified Z and W components.
    // Useful for extending 2D coordinates to 4D space.
    func fromVector2(v: Vector2, z, w: f32)->Vector4
    {
        return Vector4{v.x, v.y, z, w}
    }
}
