/**
Swag accepts copy and move semantics for structures. In this examples, we use a `Vector3` to illustrate, even if of course that kind of struct does not need a move semantic, as there's no heap involved.
*/

struct Vector3
{
    x, y, z: s32 = 666
}

impl Vector3
{
    // This is used for 'copy semantic'.
    mtd opPostCopy()
    {
        x, y, z += 1
    }

    // This is used for 'move semantic'.
    mtd opPostMove()
    {
        x, y, z += 2
    }

    // Just imagine that we have something to drop. Like a memory allocated buffer.
    mtd opDrop()
    {
    }
}

#test
{
    var a = Vector3{}
    var b = Vector3{100, 200, 300}

    // "copy semantic". The default behaviour.
    // 1. This will call 'opDrop' on 'a' if the function exists because 'a' could already be assigned.
    // 2. This will raw copy 'b' to 'a'.
    // 3. This will call 'opPostCopy' on 'a' if it exists.
    a = b
    @assert(a.x == 101) // +1 because of the call to opPostCopy
    @assert(a.y == 201)
    @assert(a.z == 301)

    // "move semantic" by adding the modifier 'move' just after '='.
    // 1. This will call 'opDrop' on 'a' if it exists
    // 2. This will raw copy 'b' to 'a'
    // 3. This will call 'opPostMove' on 'a' if it exists
    // 4. This will reinitialize 'b' to the default values if 'opDrop' exists
    a =,move b
    @assert(a.x == 102) // +2 because of the call to opPostMove
    @assert(a.y == 202)
    @assert(a.z == 302)

    // 'Vector3' contains an 'opDrop' special function, so 'b' will be reinitialized to
    // the default values after the 'move'. Default values are 666.
    @assert(b.x == 666)

    // If you know what you're doing, you can avoid the first call to 'opDrop' with '=,nodrop'
    // Do it when you know the state of 'a' and do not want an extra unnecessary call
    // (for example if 'a' is in an undetermined state).

    a =,nodrop b // Copy b to a without dropping 'a' first
    a =,nodrop,move b // Move b to a without dropping 'a' first

    // For the 'move semantic', you can avoid the last reinitialization by using 'moveraw'. Of course, do this at your own risk, if you know that 'b' will never by dropped by the compiler or if you reinitialize its state yourself.

    // instead of 'move'
    a =,moveraw b
    a =,nodrop,moveraw b
}

/**
# Move semantic
`&&` can be used instead of `&` in a function parameter to declare a **move semantic** intention.
*/

#test
{
    // This is the 'move' version of 'assign'. With 'moveref', we tell the compiler that this version will take the owership on 'from'.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: &&Vector3)
    {
        assignTo =,move from
    }

    // This is the normal 'copy' version. In this version, 'from' will not be changed, that's why it's constant (not a ref).
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: Vector3)
    {
        assignTo = from
    }

    var a  = Vector3{1, 2, 3}
    var b: Vector3

    // Call the 'copy' version of 'assign'.
    assign(&b, a)
    // As this will call 'opPostCopy', we have +1 on each field.
    @assert(b.x == 2 and b.y == 3 and b.z == 4)
    // 'a' remains unchanged
    @assert(a.x == 1 and a.y == 2 and a.z == 3)

    // Now we tell the compiler to use the 'moveref' version of 'assign'.
    assign(&b, moveref &a)
    // As 'opPostMove' has been called, we have +2 on each field.
    @assert(b.x == 3 and b.y == 4 and b.z == 5)
    // And as this is a move, then 'a' is now reinitialized to its default values.
    @assert(a.x == 666 and a.y == 666 and a.z == 666)
}
