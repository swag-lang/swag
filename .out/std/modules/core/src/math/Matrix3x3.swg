#global namespace Math
#global public

struct Matrix3x3
{
    m: [3,3] f32
}

impl Matrix3x3
{
    func setIdentity(using self)
    {
        m[0, 0] = 1; m[0, 1] = 0; m[0, 2] = 0
        m[1, 0] = 0; m[1, 1] = 1; m[1, 2] = 0
        m[2, 0] = 0; m[2, 1] = 0; m[2, 2] = 1
    }
}