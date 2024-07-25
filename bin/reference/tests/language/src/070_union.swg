/**
An union is just a struct where all fields are located at offset 0.
*/

#test
{
    union MyUnion
    {
        x, y, z: f32
    }

    var v = MyUnion{x: 666}
    @assert(v.y == 666)
    @assert(v.z == 666)
}

