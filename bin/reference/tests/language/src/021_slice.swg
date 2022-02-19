#test
{
    // A slice is a pointer on a buffer of datas, and a 'uint' to count the number of elements
    // It is declared with [..]
    {
        var a: [..] bool
        #assert @sizeof(a) == @sizeof(*void) + @sizeof(uint)
    }

    // You can initialize a slice like an array
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        @assert(@countof(a) == 5)
        @assert(a[0] == 10)
        @assert(a[4] == 50)

        // But as this is a slice and not a static array, you can change the value at runtime
        a = @[1, 2]
        @assert(@countof(a) == 2)
        @assert(a[0] == 1)
        @assert(a[1] == 2)
    }

    // '@dataof' will return the address of the values, '@countof' the number of elements
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        count := @countof(a)
        addr  := @dataof(a)
        @assert(count == 5)
        @assert(addr[0] == 10)
        @assert(addr[4] == 50)

        a = @[1, 2]
        @assert(@countof(a) == 2)
    }

    // You can create a slice with your own pointer and count using '@mkslice'
    {
        var array: [4]  u32 = @[10, 20, 30, 40]

        // Creates a slice of 'array' (a view, or subpart) starting at index 2, with 2 elements
        var slice: [..] u32 = @mkslice(&array[0] + 2, 2)

        @assert(@countof(slice) == 2)
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)

        @assert(array[2] == 30)
        slice[0] = 314
        @assert(array[2] == 314)
    }

    // For a string, the slice must be 'const' because a string is immutable
    {
        var str = "string"
        var strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
        @assert(strSlice[0] == "s"'u8)
        @assert(strSlice[1] == "t"'u8)
    }

    // Instead of '@mkslice', you can slice something with the '..' operator
    // For example you can slice a string.
    {
        var str = "string"
        var slice = str[1..3]
        @assert(slice == "tri")
    }

    // You can slice an array
    {
        arr := @[10, 20, 30, 40]
        slice := arr[2..3]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }

    // You can slice a pointer
    {
        arr := @[10, 20, 30, 40]
        ptr := &arr[2]
        slice := ptr[0..1]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }

    // You can slice another slice
    {
        arr := @[10, 20, 30, 40]
        slice1 := arr[1..3]
        @assert(slice1[0] == 20)
        @assert(slice1[1] == 30)
        @assert(slice1[2] == 40)

        slice2 := slice1[1..2]
        @assert(slice2[0] == 30)
        @assert(slice2[1] == 40)
    }
}