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

    // You can create a slice with your own pointer and count
    {
        var array: [10] u32
        var slice: [..] u32 = {&array[0], 2}
        @assert(@countof(slice) == 2)

        //var str = "string"
        //var strSlice = {@dataof(str), 2}
    }
}