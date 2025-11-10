struct Vector3 { x, y, z: s32 = 666 }

impl Vector3
{
    mtd opDrop() {}
}

//#[Swag.PrintBc]
#test
{
    var a = Vector3{}
    var b = Vector3{1, 2, 3}
    a = #move b
    @assert(b.x == 666)
}
