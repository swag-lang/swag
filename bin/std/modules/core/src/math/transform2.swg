#global namespace Math
#global public

// 2D transformation matrix structure
struct Transform2
{
    m11, m12:     f32     // First row of the 2D transformation matrix
    m21, m22:     f32     // Second row of the 2D transformation matrix
    m31, m32:     f32     // Translation components of the transformation

    #[Swag.Offset("m11")]
    m:   [2, 2] f32 // 2x2 matrix view for rotation/scale components
    #[Swag.Offset("m31")]
    tx:  f32 // Translation component along X-axis
    #[Swag.Offset("m32")]
    ty:  f32 // Translation component along Y-axis
}

impl Transform2
{
    const Identity: Transform2{1, 0, 0, 1, 0, 0} // Identity transform (no rotation, scale 1, no translation)

    // Set transform to identity
    mtd setIdentity()
    {
        .m11 = 1
        .m12 = 0
        .m21 = 0
        .m22 = 1
        .m31 = 0
        .m32 = 0
    }

    // Checks if the transform is the identity matrix
    mtd const isIdentity()->bool
    {
        return .m11 == 1 and .m12 == 0 and
               .m21 == 0 and .m22 == 1 and
               .tx == 0 and .ty == 0
    }

    // Creates a rotation matrix, with a given center of rotation
    func createRotation(angle: f32, centerX, centerY: f32)->Transform2
    {
        let s  = Math.sin(angle)
        let c  = Math.cos(angle)
        let c1 = 1.0 - c

        with var result: retval = undefined
        {
            .tx  = (centerX * c1) + (centerY * s)
            .ty  = (centerY * c1) - (centerX * s)
            .m11 = c
            .m12 = s
            .m21 = -s
            .m22 = c
        }

        return result
    }

    // Creates a scale matrix, with a given center
    func createScale(scaleX, scaleY: f32, centerX, centerY: f32)->Transform2
    {
        with var result: retval = undefined
        {
            .m11 = scaleX
            .m12 = 0
            .m21 = 0
            .m22 = scaleY
            .tx  = centerX - scaleX * centerX
            .ty  = centerY - scaleY * centerY
        }

        return result
    }

    // Creates a translation matrix
    func createTranslation(x, y: f32)->Transform2
    {
        with var result: retval = undefined
        {
            .m11 = 1
            .m12 = 0
            .m21 = 0
            .m22 = 1
            .tx  = x
            .ty  = y
        }

        return result
    }

    // Sets the translation components directly
    mtd setTranslation(translation: Vector2)
    {
        .tx = translation.x
        .ty = translation.y
    }

    // Gets the translation components
    mtd getTranslation()->Vector2
    {
        return Vector2{.tx, .ty}
    }

    // Applies the transform to a 2D point
    mtd transformPoint(point: Vector2)->Vector2
    {
        let rx = point.x * .m11 + point.y * .m21 + .tx
        let ry = point.x * .m12 + point.y * .m22 + .ty
        return Vector2{rx, ry}
    }

    // Multiplies the current transform by another transformation matrix
    mtd multiply(matrix2: Transform2)
    {
        let sm11 = .m11
        .m11 = sm11 * matrix2.m11 + .m12 * matrix2.m21
        .m12 = sm11 * matrix2.m12 + .m12 * matrix2.m22

        let sm21 = .m21
        .m21 = sm21 * matrix2.m11 + .m22 * matrix2.m21
        .m22 = sm21 * matrix2.m12 + .m22 * matrix2.m22

        let stx = .tx
        .tx = stx * matrix2.m11 + .ty * matrix2.m21 + matrix2.tx
        .ty = stx * matrix2.m12 + .ty * matrix2.m22 + matrix2.ty
    }
}
