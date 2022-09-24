#test
{
    // use 'cast(type)' to cast from one type to another
    {
        x := 1.0
        #assert @typeof(x) == f32

        y := cast(s32) x
        #assert @typeof(y) == s32
        @assert(y == 1)
    }

    // use 'acast' to automatically cast to the left expression
    {
        var x: f32 = 1.0
        var y: s32 = acast x    // cast to 's32'
        #assert @typeof(y) == s32
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

    // An implicit cast is done if there's no loss of precision.
    // In that case, you can affect different types.
    {
        var x: s16 = 1's8   // 8 bits to 16 bits, fine
        var y: s32 = 1's16  // 16 bits to 32 bits, fine

        // But the following would generate an error as you cannot affect 's16' to 's8' without
        // an explicit cast(s16)

        //var z0: s16 = 1
        //var z1: s8 = z0
    }

    // To avoid overflow with operators + - * /, operations are promoted to 32 or 64 bits
    {
        @assert(@typeof(1's8 + 2's8)   == s32)
        @assert(@typeof(1's8 + 2's16)  == s32)
        @assert(@typeof(1'u8 + 2'u8)   == u32)
        @assert(@typeof(1'u8 + 2'u16)  == u32)
        @assert(@typeof(1'u8 + 2'f64)  == f64)
    }

    // But in that case, you can still affect to the original type.
    // The compiler will add an implicit cast, and a safety check
    {
        // Even if the resulting type of the '+' operator is 's32', you can still
        // affect to a 's8' value. Swag will panic at runtime if the conversion is invalid.
        var x: s8 = 1's8 + 2's8
    }
}