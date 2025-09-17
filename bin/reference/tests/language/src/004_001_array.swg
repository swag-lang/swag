/**
# Static Arrays in Swag

Static arrays are fixed-size arrays where the size is known and set at compile time. Unlike dynamic arrays from the `Std.Core` module, static arrays do not change in size during runtime. They are ideal for situations where memory and size constraints are predictable and fixed.
*/

/**
# Declaring a Static Array

A static array is declared using the syntax `[N]` followed by the type, where `N` is the number of elements.
*/
#test
{
    var array: [2] s32               // Declare a static array with two 32-bit signed integers (s32)
    array[0] = 1                     // Assign the value 1 to the first element
    array[1] = 2                     // Assign the value 2 to the second element
}

/**
# Array Size and Memory

The `@countof` intrinsic can be used to get the number of elements in an array, while `#sizeof` provides the total size of the array in bytes.
*/
#test
{
    // Declare a static array with two 32-bit signed integer (s32) elements
    var array: [2] s32

    // Use '#typeof' to get the array type and check the number of elements with '.count'
    #assert(#typeof(array).count == 2)

    // '@countof' is a runtime intrinsic that also works with compile-time elements
    #assert(@countof(array) == 2)                // Ensure the array contains 2 elements

    // '#sizeof' is a compile-time intrinsic that returns the size in bytes of the given element
    #assert(#sizeof(array) == 2 * #sizeof(s32))  // Verify the total size in bytes
}

/**
# Obtaining the Address of an Array

The `@dataof` intrinsic retrieves the address of the first element in an array.
*/
#test
{
    var array: [2] s32               // Declare a static array of two s32 elements
    var ptr0   = @dataof(array)      // Get the base address of the array
    ptr0[0] = 1                      // Access the first element via the pointer

    // Alternatively, obtain the address of a specific element
    var ptr1 = &array[0]             // Get the address of the first element
    ptr1[1] = 2                      // Access the second element via the pointer

    @assert(array[0] == 1)           // Confirm that the first element is 1
    @assert(array[1] == 2)           // Confirm that the second element is 2
}

/**
# Array Literals

An array literal is a list of elements enclosed in square brackets `[A, B, ...]`.
*/
#test
{
    var arr = [1, 2, 3, 4]                  // Declare and initialize a static array of four s32 elements
    #assert(@countof(arr) == 4)             // Verify the array contains 4 elements
    #assert(#typeof(arr) == #type [4] s32)  // Verify the array's type is [4] s32
}

/**
# Type Deduction in Arrays

Swag can deduce the size of the array from the initialization expression, eliminating the need to specify the dimension explicitly.
*/
#test
{
    // Array dimension is deduced from the initialization with 2 elements
    var array: [?] s32 = [1, 2]
    @assert(array[0] == 1)
    @assert(array[1] == 2)
    #assert(@countof(array) == 2)

    // Both dimensions and types are deduced from the initialization expression
    var array1 = ["10", "20", "30"]
    @assert(array1[0] == "10")
    @assert(array1[1] == "20")
    @assert(array1[2] == "30")
    #assert(@countof(array1) == 3)
}

/**
# Default Initialization

Static arrays in Swag are automatically initialized to zero values (0 for integers, null for strings, false for booleans, etc.) unless specified otherwise.
*/
#test
{
    var array: [2] s32               // Declare a static array of two s32 elements
    @assert(array[0] == 0)           // Ensure the array is initialized to 0
    @assert(array[1] == 0)
}

/**
# Preventing Default Initialization

To enhance performance, you can bypass the default initialization by using `undefined`, particularly useful when you plan to manually initialize all array elements.
*/
#test
{
    var array: [100] s32 = undefined // Declare a large array without initializing it
}

/**
# Constant Arrays

Static arrays with compile-time values can be declared as constants, meaning their values cannot be altered after declaration.
*/
#test
{
    const array = [1, 2, 3, 4]        // Declare a constant static array
    #assert(array[0] == 1)            // Compile-time dereference of the first element
    #assert(array[3] == 4)            // Compile-time dereference of the fourth element
}

/**
# Type Inference from Array Literals

If the array's type is not explicitly specified, Swag infers the type based on the first literal value, which then applies to all other elements in the array.
*/
#test
{
    var arr = [1'f64, 2, 3, 4]        // All elements are inferred to be 'f64'
    #assert(@countof(arr) == 4)       // Verify the array has 4 elements
    #assert(#typeof(arr) == #type [4] f64)  // Confirm that all elements are of type f64
    @assert(arr[3] == 4.0)            // Confirm the value of the fourth element
}

/**
# Multi-Dimensional Arrays

Swag supports the declaration of multi-dimensional arrays, allowing you to work with arrays with multiple dimensions.
*/

/**
The syntax for declaring multi-dimensional arrays is `[X, Y, Z...]`, where `X`, `Y`, and `Z` represent each dimension.
*/
#test
{
    var array: [2, 2] s32            // Declare a 2x2 static array of s32
    array[0, 0] = 1                  // Assign the value 1 to the element at [0, 0]
    array[0, 1] = 2                  // Assign the value 2 to the element at [0, 1]
    array[1, 0] = 3                  // Assign the value 3 to the element at [1, 0]
    array[1, 1] = 4                  // Assign the value 4 to the element at [1, 1]
}

/**
# C/C++ Style Multi-Dimensional Arrays

Swag also allows for the declaration of multi-dimensional arrays using C/C++ syntax, where an array of arrays is created. This method is equivalent to Swag's native multi-dimensional array syntax.
*/
#test
{
    var array: [2] [2] s32           // Declare a 2x2 static array of s32 using C/C++ syntax
    array[0, 0] = 1                  // Assign the value 1 to the element at [0, 0]
    array[0, 1] = 2                  // Assign the value 2 to the element at [0, 1]
    array[1, 0] = 3                  // Assign the value 3 to the element at [1, 0]
    array[1, 1] = 4                  // Assign the value 4 to the element at [1, 1]
}

/**
# Array Size Deduction

Swag can deduce the size of arrays, including multi-dimensional arrays, directly from the initialization expression.
*/
#test
{
    var array  = [1, 2, 3, 4]        // Size deduced to be 4
    var array1 = [[1, 2], [3, 4]]    // Size deduced to be 2x2

    #assert(@countof(array) == 4)     // Verify that the size of the first array is 4
    #assert(@countof(array1) == 2)    // Verify that the outer array has 2 elements
}

/**
# Single Value Initialization

Swag allows the initialization of an entire array with a single value. This feature is available only for variables, not constants, and supports basic types such as integers, floats, strings, booleans, and runes.
*/
#test
{
    // Initialize the entire 2x2 boolean array with 'true'
    var arr: [2, 2] bool = true
    @assert(arr[0, 0] == true)
    @assert(arr[1, 1] == true)

    // Initialize the entire 5x10 string array with "string"
    var arr1: [5, 10] string = "string"
    @assert(arr1[0, 0] == "string")
    @assert(arr1[4, 9] == "string")
}
