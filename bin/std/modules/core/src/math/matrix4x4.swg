#global marked
#global namespace Math
#global public

#[Serialization.Final]
struct Matrix4x4
{
    m:   [4, 4] f32
}

impl Matrix4x4
{
    mtd setIdentity()
    {
        me.m[0, 0] = 1
        me.m[0, 1] = 0
        me.m[0, 2] = 0
        me.m[0, 3] = 0
        me.m[1, 0] = 0
        me.m[1, 1] = 1
        me.m[1, 2] = 0
        me.m[1, 3] = 0
        me.m[2, 0] = 0
        me.m[2, 1] = 0
        me.m[2, 2] = 1
        me.m[2, 3] = 0
        me.m[3, 0] = 0
        me.m[3, 1] = 0
        me.m[3, 2] = 0
        me.m[3, 3] = 1
    }
}
