#global namespace Math
#global public

struct Vector4
{
    x, y, z, w: f32
}

impl Vector4
{
    #[Swag.Complete]
    mtd opAffect(value: f32)
    {
        x, y, z, w = value
    }
}