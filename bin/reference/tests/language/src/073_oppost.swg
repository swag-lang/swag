// Swag accepts copy and move semantics for structures.

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
    }
}

#test
{
    a := Vector3{}
    b := Vector3{100, 200, 300}

    // "copy semantic"
    // 1. This will call 'opDrop' on 'a' if it exists
    // 2. This will raw copy 'b' to 'a'
    // 3. This will call 'opPostCopy' on 'a' if it exists
    a = b
    @assert(a.x == 101)
    @assert(a.y == 201)
    @assert(a.z == 301)

    // "move semantic"
    // 1. This will call 'opDrop' on 'a' if it exists
    // 2. This will raw copy 'b' to 'a'
    // 3. This will call 'opPostMove' on 'a' if it exists
    // 4. This will reinitialize 'b' to the default values if 'opDrop' exists
    a =,move b
    @assert(a.x == 102)
    @assert(a.y == 202)
    @assert(a.z == 302)

    // 'b' contains an 'opDrop' special function, so it will be initialized to
    // the default values after the 'move'
    @assert(b.x == 666)

    // If you know what you're doing, you can avoid the first call to 'opDrop' with '=,nodrop'
    // Do it when you know the state of 'a' and do not want an extra unnecessary call
    // (for example if 'a' is in an undetermined state)
    a =,nodrop b
    a =,nodrop,move b

    // For the 'move semantic', you can avoid the last reinitialization by using '=,moveraw'
    // instead of '=,move'
    a =,moveraw b
    a =,nodrop,moveraw b
}
