#global marked
#global namespace Math
#global public

#[Serialization.Final]
struct Matrix3x3
{
    m:   [3, 3] f32
}

impl Matrix3x3
{
    mtd setIdentity()
    {
        me.m[0, 0] = 1
        me.m[0, 1] = 0
        me.m[0, 2] = 0
        me.m[1, 0] = 0
        me.m[1, 1] = 1
        me.m[1, 2] = 0
        me.m[2, 0] = 0
        me.m[2, 1] = 0
        me.m[2, 2] = 1
    }
}
