/**
Sometimes it's necessary to change the type of a value.
*/

#test
{
    // Use 'cast(type)' to cast from one type to another.
    {
        x := 1.0
        #assert @typeof(x) == f32

        y := cast(s32) x
        #assert @typeof(y) == s32
        @assert(y == 1)
    }

    // Use 'acast' to automatically cast to the expression on the left.
    {
        var x: f32 = 1.0
        var y: s32 = acast x    // cast 'x' to 's32'
        #assert @typeof(y) == s32
        @assert(y == 1)
    }
}

/**
Use 'cast,bit' to convert a native type to another without converting the value.
Works only if the two types are of the same size.
*/
#test
{
    var x: f32 = 1.0
    var y: u32 = cast,bit(u32) x
    @assert(y == 0x3f800000)
    #assert cast,bit(u32) 1.0 == 0x3f800000
    #assert cast,bit(f32) 0x3f800000 == 1.0
}

/**
Swag can sometimes cast from one type to another for you.
This is an 'implicit' cast.
*/
#test
{
    // An implicit cast is done if there's no loss of precision.
    // In that case, you can affect different types.
    {
        var x: s16 = 1's8   // 8 bits to 16 bits, fine
        var y: s32 = 1's16  // 16 bits to 32 bits, fine

        // But the following would generate an error as you cannot affect 's16' to 's8' without an explicit cast(s16).

        /*
        var z0: s16 = 1
        var z1: s8 = z0
        */
    }
}
