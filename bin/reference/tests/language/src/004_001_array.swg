/**
# Static Arrays in Swag

Static arrays are fixed-size arrays where the size is known at compile time.
Unlike dynamic arrays from the 'Std.Core' module, they cannot grow or shrink
during execution. Static arrays are ideal when size and memory usage are
deterministic.
*/

/**
# Declaring a Static Array

A static array is declared using the syntax '[N] type', where 'N' is the number
of elements.
*/
#test
{
    var array: [2] s32
    array[0] = 1
    array[1] = 2
}

/**
# Array Size and Memory

The '@countof' intrinsic returns the number of elements in an array, while
'#sizeof' returns the total memory size in bytes.
*/
#test
{
    var array: [2] s32

    #assert(#typeof(array).count == 2)
    #assert(@countof(array) == 2)
    #assert(#sizeof(array) == 2 * #sizeof(s32))
}

/**
# Obtaining the Address of an Array

The '@dataof' intrinsic retrieves the address of the first element in an array.
*/
#test
{
    var array: [2] s32
    var ptr0   = @dataof(array)
    ptr0[0] = 1

    var ptr1 = &array[0]
    ptr1[1] = 2

    @assert(array[0] == 1)
    @assert(array[1] == 2)
}

/**
# Array Literals

An array literal is a list of elements enclosed in brackets '[A, B, ...]'.
*/
#test
{
    var arr = [1, 2, 3, 4]
    #assert(@countof(arr) == 4)
    #assert(#typeof(arr) == #type [4] s32)
}

/**
# Type Deduction in Arrays

Swag can deduce array size and element type from the initialization expression.
*/
#test
{
    var array: [?] s32 = [1, 2]
    @assert(array[0] == 1)
    @assert(array[1] == 2)
    #assert(@countof(array) == 2)

    var array1 = ["10", "20", "30"]
    @assert(array1[0] == "10")
    @assert(array1[1] == "20")
    @assert(array1[2] == "30")
    #assert(@countof(array1) == 3)
}

/**
# Default Initialization

Static arrays are automatically initialized with zero values (0 for numbers,
'null' for strings, 'false' for booleans, etc.) unless specified otherwise.
*/
#test
{
    var array: [2] s32
    @assert(array[0] == 0)
    @assert(array[1] == 0)
}

/**
# Preventing Default Initialization

You can skip default initialization by using 'undefined', which improves
performance when the array will be manually initialized later.
*/
#test
{
    var array: [100] s32 = undefined
}

/**
# Constant Arrays

Arrays initialized with compile-time values can be declared as 'const', making
them immutable after declaration.
*/
#test
{
    const array = [1, 2, 3, 4]
    #assert(array[0] == 1)
    #assert(array[3] == 4)
}

/**
# Type Inference from Array Literals

If no explicit type is specified, Swag infers the array’s type from the first
element. All other elements are promoted to match that type.
*/
#test
{
    var arr = [1'f64, 2, 3, 4]
    #assert(@countof(arr) == 4)
    #assert(#typeof(arr) == #type [4] f64)
    @assert(arr[3] == 4.0)
}

/**
# Multi-Dimensional Arrays

Swag supports multi-dimensional arrays using the syntax '[X, Y, Z...]', where
each number represents a dimension.
*/
#test
{
    var array: [2, 2] s32
    array[0, 0] = 1
    array[0, 1] = 2
    array[1, 0] = 3
    array[1, 1] = 4
}

/**
# C/C++ Style Multi-Dimensional Arrays

You can also declare multi-dimensional arrays using nested array syntax,
similar to C/C++.
*/
#test
{
    var array: [2] [2] s32
    array[0, 0] = 1
    array[0, 1] = 2
    array[1, 0] = 3
    array[1, 1] = 4
}

/**
# Array Size Deduction

Swag can infer the dimensions of arrays — including multi-dimensional ones —
directly from the initialization expression.
*/
#test
{
    var array  = [1, 2, 3, 4]
    var array1 = [[1, 2], [3, 4]]

    #assert(@countof(array) == 4)
    #assert(@countof(array1) == 2)
}

/**
# Single Value Initialization

An entire array can be initialized with a single value. This feature applies to
variables (not constants) and works for basic types such as integers, floats,
strings, booleans, and runes.
*/
#test
{
    var arr: [2, 2] bool = true
    @assert(arr[0, 0] == true)
    @assert(arr[1, 1] == true)

    var arr1: [5, 10] string = "string"
    @assert(arr1[0, 0] == "string")
    @assert(arr1[4, 9] == "string")
}
