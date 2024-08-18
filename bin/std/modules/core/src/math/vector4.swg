#global namespace Math
#global public

#[Serialization.Final]
struct Vector4
{
    x, y, z, w: f32
}

impl Vector4
{
    const Zero: Vector4{0, 0, 0, 0}
    const One:  Vector4{1, 1, 1, 1}

    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y, z, w = value
    }

    mtd(op: string) opUnary()->Self
    {
        #if op == "-":
            return Vector4{-x, -y, -z, -w}
        #else:
            #error "unsupported unary operation [[" ++ op ++ "]]"
    }

    // Set the vector to zero
    mtd clear()
    {
        x, y, z, w = 0
    }

    // Lerp one vector with another
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
