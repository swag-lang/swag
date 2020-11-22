#test
{
    // use 'cast(type)' to cast from one type to another
    {
        x := 1.0
        y := cast(s32) x
        @assert(@typeof(y) == s32)
        @assert(y == 1)
    }

    // use 'autocast' to automatically cast to the left expression
    {
        var x: f32 = 1.0
        var y: s32 = autocast x
        @assert(@typeof(y) == s32)
        @assert(y == 1)
    }

    // use 'bitcast' to convert a native type to another without converting the value
    // Works only if the two types are of the same size
    {
        var x: f32 = 1.0
        var y: u32 = bitcast(u32) x
        @assert(y == 0x3f800000)
        #assert bitcast(u32) 1.0 == 0x3f800000
        #assert bitcast(f32) 0x3f800000 == 1.0
    }
}