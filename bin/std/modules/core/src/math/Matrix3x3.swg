#global marked
#global namespace Math
#global public

#[Serialization.Final]
struct Matrix3x3
{
    m: [3, 3] f32
}

impl Matrix3x3
{
    mtd setIdentity()
    {
        self.m[0, 0] = 1
        self.m[0, 1] = 0
        self.m[0, 2] = 0
        self.m[1, 0] = 0
        self.m[1, 1] = 1
        self.m[1, 2] = 0
        self.m[2, 0] = 0
        self.m[2, 1] = 0
        self.m[2, 2] = 1
    }
}
