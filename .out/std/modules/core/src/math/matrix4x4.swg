#global namespace Math
#global public

struct Matrix4x4
{
    m: [4,4] f32
}

impl Matrix4x4
{
    func setIdentity(using self)
    {
        m[0, 0] = 1; m[0, 1] = 0; m[0, 2] = 0; m[0, 3] = 0;
        m[1, 0] = 0; m[1, 1] = 1; m[1, 2] = 0; m[1, 3] = 0;
        m[2, 0] = 0; m[2, 1] = 0; m[2, 2] = 1; m[2, 3] = 0;
        m[3, 0] = 0; m[3, 1] = 0; m[3, 2] = 0; m[3, 3] = 1;
    }
}