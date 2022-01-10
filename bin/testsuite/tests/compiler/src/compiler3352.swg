struct Vector3
{
    x, y, z: s32 = 666
}

impl Vector3
{
    mtd opDrop()
    {
    }
}

//#[Swag.PrintBc]
#test
{
    a := Vector3{}
    b := Vector3{1, 2, 3}
    a =,move b
    @assert(b.x == 666)
}
