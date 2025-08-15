#global marked
#global namespace Math
#global public

// 2D transformation matrix structure
struct Transform2
{
    m11, m12:     f32     // First row of the 2D transformation matrix
    m21, m22:     f32     // Second row of the 2D transformation matrix
    m31, m32:     f32     // Translation components of the transformation

    #[Swag.Offset("m11")]
    m: [2, 2] f32 // 2x2 matrix view for rotation/scale components
    #[Swag.Offset("m31")]
    tx: f32 // Translation component along X-axis
    #[Swag.Offset("m32")]
    ty: f32 // Translation component along Y-axis
}

impl Transform2
{
    const Identity: Transform2{1, 0, 0, 1, 0, 0} // Identity transform (no rotation, scale 1, no translation)

    // Set transform to identity
    mtd setIdentity()
    {
        self.m11 = 1
        self.m12 = 0
        self.m21 = 0
        self.m22 = 1
        self.m31 = 0
        self.m32 = 0
    }

    // Checks if the transform is the identity matrix
    mtd const isIdentity()->bool
    {
        return self.m11 == 1 and self.m12 == 0 and
               self.m21 == 0 and self.m22 == 1 and
               self.tx == 0 and self.ty == 0
    }

    // Creates a rotation matrix, with a given center of rotation
    func createRotation(angle: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = undefined

        let s  = Math.sin(angle)
        let c  = Math.cos(angle)
        let c1 = 1.0 - c

        result.tx = (centerX * c1) + (centerY * s)
        result.ty = (centerY * c1) - (centerX * s)

        result.m11 = c
        result.m12 = s
        result.m21 = -s
        result.m22 = c

        return result
    }

    // Creates a scale matrix, with a given center
    func createScale(scaleX, scaleY: f32, centerX, centerY: f32)->Transform2
    {
        var result: retval = undefined

        result.m11 = scaleX
        result.m12 = 0
        result.m21 = 0
        result.m22 = scaleY
        result.tx  = centerX - scaleX * centerX
        result.ty  = centerY - scaleY * centerY

        return result
    }

    // Creates a translation matrix
    func createTranslation(x, y: f32)->Transform2
    {
        var result: retval = undefined

        result.m11 = 1
        result.m12 = 0
        result.m21 = 0
        result.m22 = 1
        result.tx  = x
        result.ty  = y

        return result
    }

    // Sets the translation components directly
    mtd setTranslation(translation: Vector2)
    {
        self.tx = translation.x
        self.ty = translation.y
    }

    // Gets the translation components
    mtd getTranslation()->Vector2
    {
        return Vector2{self.tx, self.ty}
    }

    // Applies the transform to a 2D point
    mtd transformPoint(point: Vector2)->Vector2
    {
        let rx = point.x * self.m11 + point.y * self.m21 + self.tx
        let ry = point.x * self.m12 + point.y * self.m22 + self.ty
        return Vector2{rx, ry}
    }

    // Multiplies the current transform by another transformation matrix
    mtd multiply(matrix2: Transform2)
    {
        let sm11 = self.m11
        self.m11 = sm11 * matrix2.m11 + self.m12 * matrix2.m21
        self.m12 = sm11 * matrix2.m12 + self.m12 * matrix2.m22

        let sm21 = self.m21
        self.m21 = sm21 * matrix2.m11 + self.m22 * matrix2.m21
        self.m22 = sm21 * matrix2.m12 + self.m22 * matrix2.m22

        let stx = self.tx
        self.tx = stx * matrix2.m11 + self.ty * matrix2.m21 + matrix2.tx
        self.ty = stx * matrix2.m12 + self.ty * matrix2.m22 + matrix2.ty
    }
}
