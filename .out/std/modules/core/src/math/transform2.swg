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

impl Transform2
{
    const Identity: Transform2{1, 0, 0, 1, 0, 0}

    // Set transform to identity
    func setIdentity(using self)
    {
        m11 = 1; m12 = 0
        m21 = 0; m22 = 1
        m31 = 0; m32 = 0
    }

    // Creates a rotation matix, with a given center of rotation
    func createRotation(angle: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = ?
        using result

        s := Math.sin(angle)
        c := Math.cos(angle)
        tx = (centerX * (1.0 - c)) + (centerY * s)
        ty = (centerY * (1.0 - c)) - (centerX * s)

        m11 = c
        m12 = s
        m21 = -s
        m22 = c
        tx  = 0
        ty  = 0

        return result
    }

    // Creates a scale matix, with a given center
    func createScale(scaleX, scaleY: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = ?
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
        var result: retval = ?
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
    func multiply(using self, matrix2: Transform2)
    {
        sm11 := m11
        m11 = sm11 * matrix2.m11 + m12 * matrix2.m21
        m12 = sm11 * matrix2.m12 + m12 * matrix2.m22

        sm21 := m21
        m21 = m21 * matrix2.m11 + m22 * matrix2.m21
        m22 = sm21 * matrix2.m12 + m22 * matrix2.m22

        tx = tx * matrix2.m11 + ty * matrix2.m21 + matrix2.tx
        ty = tx * matrix2.m12 + ty * matrix2.m22 + matrix2.ty
    }
}