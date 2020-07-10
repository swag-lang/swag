#test
{
    // A slice is a pointer on datas, and a 'u64' for the count
    {
        var a: [..] bool
        @assert(@sizeof(a) == @sizeof(*void) + @sizeof(u64))
    }

    // You can initialize a slice like an array
    {
        var a: [..] u32 = [10, 20, 30, 40, 50]
        @assert(@countof(a) == 5)
    }

    // You can create a slice with your own pointer and count using a tuple
    {
        var array: [4] u32 = [10, 20, 30, 40]
        var slice: [..] u32 = {&array[0] + 2, 2}
        @assert(@countof(slice) == 2)
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)

        @assert(array[2] == 30)
        slice[0] = 314
        @assert(array[2] == 314)
    }

    // For a string, slice must be 'const'
    {
        var str = "string"
        var strSlice: const [..] u8 = {@dataof(str), 2}
        @assert(strSlice[0] == "s"'u8)
        @assert(strSlice[1] == "t"'u8)
    }
}