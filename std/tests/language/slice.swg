#test
{
    // A slice is a pointer on datas, and a 'u64' for the count
    {
        var a: [..] bool
        @assert(@sizeof(a) == @sizeof(*void) + @sizeof(u64))
    }

    // You can initialize a slice like an array
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        @assert(@countof(a) == 5)
        @assert(a[0] == 10)
        @assert(a[4] == 50)
    }

    // '@dataof' will return the address of the values, '@countof' the number of elements
    {
        var a: const [..] u32 = @[10, 20, 30, 40, 50]
        count := @countof(a)
        @assert(count == 5)
        addr := @dataof(a)
        @assert(addr[0] == 10)
        @assert(addr[4] == 50)

        a = @[1, 2]
        @assert(@countof(a) == 2)
    }

    // You can create a slice with your own pointer and count using '@mkslice'
    {
        var array: [4] u32 = @[10, 20, 30, 40]
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

    // You can slice a string with the '..' operator
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