/**
A slice in Swag is a flexible view over a contiguous block of memory. Unlike static arrays, slices can be resized or point to different segments of memory at runtime. A slice consists of a pointer to the data buffer and a `u64` value that stores the number of elements in the slice.

This makes slices a powerful tool for working with subsets of data without copying it, allowing efficient manipulation of large datasets.
*/

#test
{
    var a: [..] bool // Declare a slice of boolean values
    #assert @sizeof(a) == @sizeof(*void) + @sizeof(u64)  // Verify that the size of the slice includes the pointer and the element count
}

/**
Slices can be initialized similarly to arrays. You can assign an array literal to a slice, and the slice will point to the elements in the array.
*/

#test
{
    var a: const [..] u32 = [10, 20, 30, 40, 50]  // Initialize a slice with an array literal
    @assert(@countof(a) == 5)  // The slice contains 5 elements
    @assert(a[0] == 10)
    @assert(a[4] == 50)

    // Slices are dynamic, so you can change the elements at runtime.
    a = [1, 2]  // Reassign the slice to a different array
    @assert(@countof(a) == 2)
    @assert(a[0] == 1)
    @assert(a[1] == 2)
}

/**
At runtime, you can use `@dataof` to retrieve the address of the data buffer and `@countof` to get the number of elements in the slice.
*/

#test
{
    var a: const [..] u32 = [10, 20, 30, 40, 50]  // Initialize a slice with an array literal
    let count = @countof(a)  // Get the number of elements in the slice
    let addr  = @dataof(a)   // Get the address of the slice's data buffer
    @assert(count == 5)
    @assert(addr[0] == 10)
    @assert(addr[4] == 50)

    a = [1, 2]  // Reassign the slice to a different array
    @assert(@countof(a) == 2)
}

/**
You can create a slice with your own pointer and count using `@mkslice`.
*/

#test
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

/**
For a string, the slice must be `const` because a string is immutable.
*/

#test
{
    let str       = "string"
    let strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
    @assert(strSlice[0] == `s`)
    @assert(strSlice[1] == `t`)
}

/**
Instead of `@mkslice`, you can slice something with the `..` operator. For example, you can slice a string.
*/

#test
{
    let str = "string"

    // Creates a slice starting at byte 0 and ending at byte 3.
    let slice = str[1..3]

    @assert(slice == "tri")
}

/**
The upper limit is **included** by default. If you want to exclude it, use `..<` instead of `..`.
*/

#test
{
    let str   = "string"
    let slice = str[1..<3]
    @assert(slice == "tr")
}

/**
You can omit the upper bound if you want to slice to the end.
*/

#test
{
    let str   = "string"
    let slice = str[2..]
    @assert(slice == "ring")
}

/**
You can omit the lower bound if you want to slice from the start (0).
*/

#test
{
    let str   = "string"
    let slice = str[..2]     // Index 2 is included
    @assert(slice == "str")
    let slice1 = str[..<2] // Index 2 is excluded
    @assert(slice1 == "st")
}

/**
You can also slice an array.
*/

#test
{
    let arr   = [10, 20, 30, 40]
    let slice = arr[2..3]
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)
    @assert(@countof(slice) == 2)

    // Creates a slice for the whole array
    let slice1 = arr[..]
    @assert(@countof(slice1) == @countof(arr))
}

/**
You can slice another slice.
*/

#test
{
    let arr    = [10, 20, 30, 40]
    let slice1 = arr[1..3]
    @assert(slice1[0] == 20)
    @assert(slice1[1] == 30)
    @assert(slice1[2] == 40)

    let slice2 = slice1[1..2]
    @assert(slice2[0] == 30)
    @assert(slice2[1] == 40)
}

/**
You can transform a pointer to a slice.
*/

#test
{
    var arr   = [10, 20, 30, 40]
    let ptr   = &arr[2]
    let slice = ptr[0..1]
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)
    @assert(@countof(slice) == 2)
}
