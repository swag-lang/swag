#global namespace Math
#global public

struct Transform2
{
    m11, m12:   f32
    m21, m22:   f32
    m31, m32:   f32

    #[Swag.offset("m11")]
    m:  [2,2] f32
    #[Swag.offset("m31")]
    tx: f32
    #[Swag.offset("m32")]
    ty: f32
}

#print @sizeof(Transform2)

impl Transform2
{
    // Set transform to identity
    func setIdentity(using self)
    {
        m11 = 1; m12 = 0
        m21 = 0; m22 = 1
        m31 = 0; m32 = 0
    }
}