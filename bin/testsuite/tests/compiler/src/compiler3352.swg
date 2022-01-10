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
    #if #cfg == "release" or #cfg == "fast-debug"
    {
        //@print("x")
        @assert(b.x != 666) // opDrop should have been removed by optim, so reinit should not be done
    }
}
