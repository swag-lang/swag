
#test
{
    {
        union MyUnion
        {
            x, y, z: f32
        }

        var v = MyUnion{x: 666}
        @assert(v.y == 666)
        @assert(v.z == 666)
    }
}
