#test
{
    // It is declared with '[..]'.
    {
        var a: [..] bool
        #assert(#sizeof(a) == #sizeof(*void) + #sizeof(u64))
    }

    // You can initialize a slice like an array.
    {
        var a: const [..] u32 = [10, 20, 30, 40, 50]
        @assert(@countof(a) == 5)
        @assert(a[0] == 10)
        @assert(a[4] == 50)

        // But as this is a slice and not a static array, you can change the value at runtime.
        // So now 'a' is a slice of two s32 values.
        a = [1, 2]
        @assert(@countof(a) == 2)
        @assert(a[0] == 1)
        @assert(a[1] == 2)
    }

    // At runtime, `@dataof` will return the address of the values, `@countof` the number of elements.
    {
        var a:    const [..] u32 = [10, 20, 30, 40, 50]
        let count = @countof(a)
        let addr  = @dataof(a)
        @assert(count == 5)
        @assert(addr[0] == 10)
        @assert(addr[4] == 50)

        a = [1, 2]
        @assert(@countof(a) == 2)
    }

    // You can create a slice with your own 'pointer' and 'count' using `@mkslice`.
    {
        var array: [4] u32 = [10, 20, 30, 40]

        // Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements.
        let slice: [..] u32 = @mkslice(&array[0] + 2, 2)

        @assert(@countof(slice) == 2)
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)

        @assert(array[2] == 30)
        slice[0] = 314
        @assert(array[2] == 314)
    }

    // For a string, the slice must be 'const' because a string is immutable.
    {
        let str       = "string"
        let strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
        @assert(strSlice[0] == 's''u8)
        @assert(strSlice[1] == 't''u8)
    }
}
