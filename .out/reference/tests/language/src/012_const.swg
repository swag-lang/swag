#test
{
    // If you use 'const' instead of 'var', the value must be known by the compiler
    // There's no memory footprint if the type is a value or a string
    {
        const a = 666
        #assert(a == 666)

        const b: string = "string"
        #assert(b == "string")
    }

    // Const can be more than just a simple type.
    // In that case, there's a memory footprint, because the constant is stored in the data segment.
    // That means that you can take the address of such a constant
    {
        const a: [3] s32 = @[0, 1, 2]
        ptr := &a[0]
        @assert(ptr[0] == 0)
        @assert(ptr[2] == 2)
    }
}