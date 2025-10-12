/**
# Slices in Swag

A slice provides a dynamic view over a contiguous block of memory. Unlike static arrays,
slices can point to different memory regions or subsets of existing data at runtime.

A slice consists of a data pointer and a 'u64' count representing the number of elements.
This allows efficient access to large datasets without copying memory.
*/
#test
{
    var a: [..] bool
    #assert(#sizeof(a) == #sizeof(*void) + #sizeof(u64))
}

/**
# Initializing Slices

Slices can be initialized directly with array literals. The slice will reference
the array’s elements.
*/
#test
{
    var a: const [..] u32 = [10, 20, 30, 40, 50]
    @assert(@countof(a) == 5)
    @assert(a[0] == 10)
    @assert(a[4] == 50)

    a = [1, 2]
    @assert(@countof(a) == 2)
    @assert(a[0] == 1)
    @assert(a[1] == 2)
}

/**
# Accessing Slice Data

The '@dataof' intrinsic retrieves the slice’s data address, and '@countof' returns
the number of elements.
*/
#test
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

/**
# Creating Slices with '@mkslice'

The '@mkslice' intrinsic creates a slice from a pointer and an element count.
*/
#test
{
    var array: [4] u32 = [10, 20, 30, 40]

    let slice: [..] u32 = @mkslice(&array[0] + 2, 2)
    @assert(@countof(slice) == 2)
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)

    slice[0] = 314
    @assert(array[2] == 314)
}

/**
# Slicing Strings

Strings can be sliced, but the result must be declared as 'const' since strings are immutable.
*/
#test
{
    let str       = "string"
    let strSlice: const [..] u8 = @mkslice(@dataof(str), 2)
    @assert(strSlice[0] == 's')
    @assert(strSlice[1] == 't')
}

/**
# Slicing with the '..' Operator

The '..' operator can be used to create slices directly.
*/
#test
{
    let str   = "string"
    let slice = str[1 to 3]
    @assert(slice == "tri")
}

/**
# Inclusive and Exclusive Slicing

By default, the upper bound in a slice is inclusive. To exclude it, use 'until' instead of 'to'.
*/
#test
{
    let str   = "string"
    let slice = str[1 until 3]
    @assert(slice == "tr")
}

/**
# Slicing to the End

Omitting the upper bound creates a slice extending to the end of the sequence.
*/
#test
{
    let str   = "string"
    let slice = str[2 to ]
    @assert(slice == "ring")
}

/**
# Slicing from the Start

Omitting the lower bound starts the slice from index 0.
*/
#test
{
    let str = "string"

    let slice = str[ to 2]
    @assert(slice == "str")

    let slice1 = str[ until 2]
    @assert(slice1 == "st")
}

/**
# Slicing Arrays

Arrays can be sliced the same way as strings.
*/
#test
{
    let arr   = [10, 20, 30, 40]
    let slice = arr[2 to 3]
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)
    @assert(@countof(slice) == 2)

    let slice1 = arr[ to ]
    @assert(@countof(slice1) == @countof(arr))
}

/**
# Slicing a Slice

A slice can be further sliced to produce another slice.
*/
#test
{
    let arr    = [10, 20, 30, 40]
    let slice1 = arr[1 to 3]
    @assert(slice1[0] == 20)
    @assert(slice1[1] == 30)
    @assert(slice1[2] == 40)

    let slice2 = slice1[1 to 2]
    @assert(slice2[0] == 30)
    @assert(slice2[1] == 40)
}

/**
# Transforming a Pointer into a Slice

A pointer can be transformed into a slice by specifying a range.
*/
#test
{
    var arr   = [10, 20, 30, 40]
    let ptr   = &arr[2]
    let slice = ptr[0 to 1]
    @assert(slice[0] == 30)
    @assert(slice[1] == 40)
    @assert(@countof(slice) == 2)
}
