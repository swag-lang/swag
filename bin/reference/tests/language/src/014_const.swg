/**
If you use `const` instead of `var`, the value must be known by the compiler. There's no memory footprint if the type is a value or a string.
*/

#test
{
    // These are constants and not variables. So they cannot be changed after the declaration.
    const a = 666
    #assert a == 666

    const b: string = "string"
    #assert b == "string"
}

/**
Constants can have more than just simple types. In that case, there's a memory footprint, because those constants are stored in the data segment. But that means also you could take the address of such constants at runtime.
*/
#test
{
    // This is our first static array. It contains '3' elements, and the type of the elements is `s32`.
    {
        const a: [3] s32 = [0, 1, 2]

        ptr := &a[0] // Take the address of the first element
        @assert(ptr[0] == 0)
        @assert(ptr[2] == 2)

        // But as this is a constant, we can also test the values at compile time.
        #assert a[0] == 0
        #assert a[1] == 1
        #assert a[2] == 2
    }

    // An example of a multidimensional array as a constant. We will detail arrays later.
    {
        const M4x4: [4, 4] f32 = [
            [1, 0, 0, 0],
            [0, 1, 0, 0],
            [0, 0, 1, 0],
            [0, 0, 0, 1]
        ]
    }
}