#test
{
    // ...

    // A struct can be anonymous when declared as a variable type
    {
        var tuple: struct
        {
            x: f32
            y: f32
        }
        var tuple1: struct{x, y: f32; }

        tuple.x = 1.0
        tuple.y = 2.0
        @assert(tuple.x == 1.0)
        @assert(tuple.y == 2.0)
    }
}