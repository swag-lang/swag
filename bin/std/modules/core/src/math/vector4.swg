#global namespace Math
#global public

// A 4D vector struct representing a point or direction in 4D space.
// Contains x, y, z, and w components as 32-bit floating point values.
// Commonly used for homogeneous coordinates in 3D graphics or RGBA color values.
#[Serialization.Final]
struct Vector4
{
    x:     f32     // X component of the vector
    y:     f32     // Y component of the vector
    z:     f32     // Z component of the vector
    w:     f32     // W component of the vector
}

impl Vector4
{
    const Zero: Vector4{0, 0, 0, 0}     // Zero vector constant with all components set to 0
    const One:  Vector4{1, 1, 1, 1}     // Unit vector constant with all components set to 1

    // Set all components to the same scalar value.
    // This allows assignment like 'vec = 5.0' to set all components to 5.0.
    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y, z, w = value
    }

    // Apply unary operations to the vector.
    // Currently supports negation (-) to return a vector with all components negated.
    mtd(op: string) opUnary()->Self
    {
        #if op == "-":
            return Vector4{-x, -y, -z, -w}
        #else:
            #error("unsupported unary operation [[" ++ op ++ "]]")
    }

    // Set all vector components to zero.
    // Equivalent to assigning the 'Zero' constant but modifies the current instance.
    mtd clear()
    {
        x, y, z, w = 0
    }

    // Perform linear interpolation between two vectors.
    // Interpolates each component separately using the given 'factor' (0.0 to 1.0).
    // When 'factor' is 0.0, returns 'src'; when 1.0, returns 'dst'.
    func lerp(src, dst: Vector4, factor: f32)->Vector4
    {
        var result: retval = undefined
        result.x = Math.lerp(src.x, dst.x, factor)
        result.y = Math.lerp(src.y, dst.y, factor)
        result.z = Math.lerp(src.z, dst.z, factor)
        result.w = Math.lerp(src.w, dst.w, factor)
        return result
    }
}
