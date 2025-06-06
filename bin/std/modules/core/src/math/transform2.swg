#global namespace Math
#global public

struct Transform2
{
    m11, m12:     f32
    m21, m22:     f32
    m31, m32:     f32

    #[Swag.Offset("m11")]
    m: [2, 2] f32
    #[Swag.Offset("m31")]
    tx: f32
    #[Swag.Offset("m32")]
    ty: f32
}

impl Transform2
{
    const Identity: Transform2{1, 0, 0, 1, 0, 0}

    // Set transform to identity
    mtd setIdentity()
    {
        m11 = 1
        m12 = 0
        m21 = 0
        m22 = 1
        m31 = 0
        m32 = 0
    }

    // Creates a rotation matix, with a given center of rotation
    func createRotation(angle: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = undefined
        using result

        let s  = Math.sin(angle)
        let c  = Math.cos(angle)
        let c1 = 1.0 - c

        tx = (centerX * c1) + (centerY * s)
        ty = (centerY * c1) - (centerX * s)

        m11 = c
        m12 = s
        m21 = -s
        m22 = c

        return result
    }

    // Creates a scale matix, with a given center
    func createScale(scaleX, scaleY: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = undefined
        using result

        m11 = scaleX
        m12 = 0
        m21 = 0
        m22 = scaleY
        tx  = centerX - scaleX * centerX
        ty  = centerY - scaleY * centerY

        return result
    }

    // Creates a translation matix
    func createTranslation(x, y: f32)->Transform2
    {
        var result: retval = undefined
        using result

        m11 = 1
        m12 = 0
        m21 = 0
        m22 = 1
        tx  = x
        ty  = y

        return result
    }

    // Multiply two matricies
    mtd multiply(matrix2: Transform2)
    {
        let sm11 = m11
        m11 = sm11 * matrix2.m11 + m12 * matrix2.m21
        m12 = sm11 * matrix2.m12 + m12 * matrix2.m22

        let sm21 = m21
        m21 = sm21 * matrix2.m11 + m22 * matrix2.m21
        m22 = sm21 * matrix2.m12 + m22 * matrix2.m22

        let stx = tx
        tx = stx * matrix2.m11 + ty * matrix2.m21 + matrix2.tx
        ty = stx * matrix2.m12 + ty * matrix2.m22 + matrix2.ty
    }
}
