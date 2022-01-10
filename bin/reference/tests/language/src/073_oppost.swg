struct Vector3
{
    x, y, z: s32 = 666
}

impl Vector3
{
    // This is used for 'copy semantic'
    mtd opPostCopy()
    {
        x, y, z += 1
    }

    // This is used for 'move semantic'
    mtd opPostMove()
    {
        x, y, z += 2
    }

    mtd opDrop()
    {
        x, y, z = 555
    }
}

#test
{
    a := Vector3{}
    b := Vector3{100, 200, 300}

    // This will make a raw copy of 'b' to 'a', then this will call 'opPostCopy' on 'a'
    // if it exists.
    // This is "copy semantic".
    a = b
    @assert(a.x == 101)
    @assert(a.y == 201)
    @assert(a.z == 301)

    // This will make a raw copy of 'b' to 'a', then this will call 'opPostMove' on 'a'
    // if it exists.
    // 'b' will then be reinitialized to its default values if necessary (if 'opDrop' is defined)
    // This is "move semantic".
    a =,move b
    @assert(a.x == 102)
    @assert(a.y == 202)
    @assert(a.z == 302)

    // 'b' contains an 'opDrop' special function, so it will be initialized to
    // the default values after the 'move'
    @assert(b.x == 666)
}
