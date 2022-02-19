#test
{
    // If you use 'const' instead of 'var', the value must be known by the compiler
    // There's no memory footprint if the type is a value or a string
    {
        const a = 666
        #assert a == 666

        const b: string = "string"
        #assert b == "string"
    }

    // Const can be more than just a simple type.
    // In that case, there's a memory footprint, because the constant is stored in the data segment.
    // That means that you can take the address of such a constant at runtime
    {
        const a: [3] s32 = @[0, 1, 2]   // static array of size 3, type s32

        ptr := &a[0] // Take the address of the first element
        @assert(ptr[0] == 0)
        @assert(ptr[2] == 2)

        #assert a[0] == 0
        #assert a[1] == 1
        #assert a[2] == 2
    }

    // A multidimensional array as a constant
    {
        const M4x4: [4, 4] f32 = @[
            @[1, 0, 0, 0],
            @[0, 1, 0, 0],
            @[0, 0, 1, 0],
            @[0, 0, 0, 1]
        ]
    }
}