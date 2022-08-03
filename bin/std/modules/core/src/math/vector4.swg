#global namespace Math
#global public

#[Serialization.PodFinal]
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
        #if   op == "-" return Vector4{-x, -y, -z, -w}
        #else #assert(false, "unsupported unary operation '" ++ op ++ "'")
    }

    // Set the vector to zero
    mtd clear()
    {
        x, y, z, w = 0
    }
}