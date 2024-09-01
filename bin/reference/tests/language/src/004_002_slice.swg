/**
# Slices in Swag

A slice in Swag provides a dynamic view over a contiguous block of memory. Unlike static arrays, slices 
can be resized or point to different segments of memory at runtime. A slice is composed of a pointer to 
the data buffer and a `u64` value that stores the number of elements in the slice.

This design makes slices a powerful tool for working with subsets of data without the need to copy it, 
enabling efficient manipulation of large datasets.
*/

#test
{
    var a: [..] bool  // Declare a slice of boolean values
    #assert #sizeof(a) == #sizeof(*void) + #sizeof(u64)  // Ensure slice size includes pointer and element count
}

/**
# Initializing Slices

Slices can be initialized in a similar manner to arrays. By assigning an array literal to a slice, 
the slice will reference the elements in the array.
*/

#test
{
    var a: const [..] u32 = [10, 20, 30, 40, 50]  // Initialize a slice with an array literal
    @assert(@countof(a) == 5)                     // Verify that the slice contains 5 elements
    @assert(a[0] == 10)
    @assert(a[4] == 50)

    // Slices are dynamic, allowing for modification of the referenced elements at runtime.
    a = [1, 2]                                    // Reassign the slice to a different array
    @assert(@countof(a) == 2)
    @assert(a[0] == 1)
    @assert(a[1] == 2)
}

/**
# Accessing Slice Data

At runtime, you can use `@dataof` to retrieve the address of the data buffer and `@countof` to 
get the number of elements in the slice.
*/

#test
{
    var a: const [..] u32 = [10, 20, 30, 40, 50]  // Initialize a slice with an array literal
    let count = @countof(a)                       // Get the number of elements in the slice
    let addr  = @dataof(a)                        // Get the address of the slice's data buffer
    @assert(count == 5)
    @assert(addr[0] == 10)
    @assert(addr[4] == 50)

    a = [1, 2]                                    // Reassign the slice to a different array
    @assert(@countof(a) == 2)
}

/**
# Creating Slices with `@mkslice`

The `@mkslice` function allows you to create a slice using your own pointer and element count.
*/

#test
{
    var array: [4] u32 = [10, 20, 30, 40]

    // Create a slice of 'array' starting at index 2, with 2 elements.
    let slice: [..] u32 = @mkslice(&array[0] + 2, 2)

    @assert(@countof(slice) == 2)
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)

    @assert(array[2] == 30)
    slice[0] = 314                                 // Modify the original array via the slice
    @assert(array[2] == 314)
}

/**
# Slicing Strings

For strings, the slice must be `const` because strings are immutable.
*/

#test
{
    let str = "string"
    let strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
    @assert(strSlice[0] == `s`)
    @assert(strSlice[1] == `t`)
}

/**
# Slicing with the `..` Operator

You can create a slice using the `..` operator instead of `@mkslice`. For instance, 
you can slice a string.
*/

#test
{
    let str = "string"

    // Create a slice starting at byte 0 and ending at byte 3.
    let slice = str[1..3]

    @assert(slice == "tri")
}

/**
# Inclusive and Exclusive Slicing

By default, the upper limit in a slice is **inclusive**. To exclude the upper limit, use `..<` 
instead of `..`.
*/

#test
{
    let str   = "string"
    let slice = str[1..<3]
    @assert(slice == "tr")
}

/**
# Slicing to the End

You can omit the upper bound to create a slice that extends to the end of the sequence.
*/

#test
{
    let str   = "string"
    let slice = str[2..]
    @assert(slice == "ring")
}

/**
# Slicing from the Start

To create a slice from the start (index 0), you can omit the lower bound.
*/

#test
{
    let str    = "string"
    let slice  = str[..2]     // Index 2 is included
    @assert(slice == "str")

    let slice1 = str[..<2]    // Index 2 is excluded
    @assert(slice1 == "st")
}

/**
# Slicing Arrays

Arrays can also be sliced similarly to strings.
*/

#test
{
    let arr   = [10, 20, 30, 40]
    let slice = arr[2..3]
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)
    @assert(@countof(slice) == 2)

    // Create a slice for the entire array.
    let slice1 = arr[..]
    @assert(@countof(slice1) == @countof(arr))
}

/**
# Slicing a Slice

It is possible to create a slice from another slice.
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
# Transforming a Pointer to a Slice

You can transform a pointer into a slice.
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
