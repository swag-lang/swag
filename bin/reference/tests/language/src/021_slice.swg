/**
A slice is a pointer on a buffer of datas, and a `u64` to count the number of elements. Unlike a static array, its value can be changed at runtime.
*/

#test
{
    // It is declared with `[..]`.
    {
        var a: [..] bool    // Slice of bools
        #assert @sizeof(a) == @sizeof(*void) + @sizeof(u64)
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
        var a: const [..] u32 = [10, 20, 30, 40, 50]
        let count = @countof(a)
        let addr = @dataof(a)
        @assert(count == 5)
        @assert(addr[0] == 10)
        @assert(addr[4] == 50)

        a = [1, 2]
        @assert(@countof(a) == 2)
    }

    // You can create a slice with your own `pointer` and `count` using `@mkslice`.
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

    // For a string, the slice must be `const` because a string is immutable.
    {
        let str = "string"
        let strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
        @assert(strSlice[0] == `s`)
        @assert(strSlice[1] == `t`)
    }
}

/**
# The slicing operator
*/
#test
{
    // Instead of `@mkslice`, you can slice something with the `..` operator. For example you can slice a string.
    {
        let str = "string"

        // Creates a slice starting at byte 0 and ending at byte 3.
        let slice = str[1..3]

        @assert(slice == "tri")
    }

    // The upper limit is **included** by default. If you want to exclude it, use `..<` insteand of `..`.
    {
        let str = "string"
        let slice = str[1..<3]
        @assert(slice == "tr")
    }

    // You can omit the upper bound if you want to slice to the end.
    {
        let str = "string"
        let slice = str[2..]
        @assert(slice == "ring")
    }

    // You can omit the lower bound if you want to slice from the start (0).
    {
        let str = "string"
        let slice = str[..2]        // Index 2 is included
        @assert(slice == "str")
        let slice1 = str[..<2]      // Index 2 is excluded
        @assert(slice1 == "st")
    }

    // You can also slice an array.
    {
        let arr = [10, 20, 30, 40]
        let slice = arr[2..3]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)

        // Creates a slice for the whole array
        let slice1 = arr[..]
        @assert(@countof(slice1) == @countof(arr))
    }

    // You can slice another slice.
    {
        let arr = [10, 20, 30, 40]
        let slice1 = arr[1..3]
        @assert(slice1[0] == 20)
        @assert(slice1[1] == 30)
        @assert(slice1[2] == 40)

        let slice2 = slice1[1..2]
        @assert(slice2[0] == 30)
        @assert(slice2[1] == 40)
    }

    // You can transform a pointer to a slice.
    {
        var arr = [10, 20, 30, 40]
        let ptr = &arr[2]
        let slice = ptr[0..1]
        @assert(slice[0] == 30)
        @assert(slice[1] == 40)
        @assert(@countof(slice) == 2)
    }
}