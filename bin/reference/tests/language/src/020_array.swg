/**
Remember that dynamic arrays are part of the `Std.Core` module. Here we are only discussing native static arrays, which are arrays with fixed size determined at compile time.
*/

/**
A static array is declared using the syntax `[N]` followed by the type, where `N` represents the number of elements in the array (the dimension).
*/
#test
{
    var array: [2] s32  // Declare a static array of two 32-bit signed integers (s32)
    array[0] = 1  // Assign the first element
    array[1] = 2  // Assign the second element
}

/**
You can determine the number of elements in an array using the `@countof` intrinsic and the size of the array in bytes using `@sizeof`.
*/
#test
{
    var array: [2] s32  // Declare a static array of two s32 elements
    #assert @countof(array) == 2  // Verify the array contains 2 elements
    #assert @sizeof(array) == 2 * @sizeof(s32)  // Verify the size in bytes (2 elements * size of s32)
}

/**
To obtain the address of an array, you can use the `@dataof` intrinsic.
*/
#test
{
    var array: [2] s32  // Declare a static array of two s32 elements
    var ptr0   = @dataof(array)  // Get the address of the array
    ptr0[0] = 1  // Access the first element through the pointer

    // You can also take the address of the first element directly
    var ptr1 = &array[0]
    ptr1[1] = 2  // Access the second element through the pointer

    @assert(array[0] == 1)
    @assert(array[1] == 2)
}

/**
An **array literal** is a list of elements enclosed in square brackets `[A, B, ...]`.
*/
#test
{
    var arr = [1, 2, 3, 4]  // Declare and initialize an array of four s32 elements
    #assert @countof(arr) == 4  // Verify the array contains 4 elements
    #assert @typeof(arr) == #type [4] s32  // Verify the array's type
}

/**
The size of the array can be deduced from the initialization expression, meaning you don't have to specify the dimension if it can be inferred.
*/
#test
{
    // The dimension is deduced from the initialization with 2 elements
    var array: [] s32 = [1, 2]
    @assert(array[0] == 1)
    @assert(array[1] == 2)
    #assert @countof(array) == 2

    // Both dimensions and types are deduced from the initialization expression
    var array1 = ["10", "20", "30"]
    @assert(array1[0] == "10")
    @assert(array1[1] == "20")
    @assert(array1[2] == "30")
    #assert @countof(array1) == 3
}

/**
Like other types in Swag, an array is initialized by default with zero values (0 for integers, null for strings, false for booleans, etc.).
*/
#test
{
    var array: [2] s32  // Declare a static array of two s32 elements
    @assert(array[0] == 0)  // Default initialization to 0
    @assert(array[1] == 0)
}

/**
For performance reasons, you can prevent an array from being initialized by using `undefined`. This is useful when you know the array will be fully initialized manually and you want to avoid the cost of default initialization.
*/
#test
{
    var array: [100] s32 = undefined  // Declare a large array without initializing it
}

/**
A static array with compile-time values can be stored as a constant, meaning it cannot be modified after its declaration.
*/
#test
{
    const array = [1, 2, 3, 4]  // Declare a constant array
    #assert array[0] == 1  // Dereference is done at compile time
    #assert array[3] == 4
}

/**
If the type of the array is not explicitly specified, the type of the **first** literal value will be used for all other elements in the array.
*/
#test
{
    var arr = [1'f64, 2, 3, 4]  // All values are deduced to be 'f64'
    #assert @countof(arr) == 4
    #assert @typeof(arr) == #type [4] f64  // Verify that all elements are of type f64
    @assert(arr[3] == 4.0)
}

/**
Swag supports multi-dimensional arrays, allowing you to declare arrays with multiple dimensions.
*/

/**
The syntax for multi-dimensional arrays is `[X, Y, Z...]`, where `X`, `Y`, and `Z` are the dimensions.
*/
#test
{
    var array: [2, 2] s32  // Declare a 2x2 array of s32
    array[0, 0] = 1  // Access and assign the first element
    array[0, 1] = 2  // Access and assign the second element
    array[1, 0] = 3  // Access and assign the third element
    array[1, 1] = 4  // Access and assign the fourth element
}

/**
Swag also accepts the C/C++ syntax for declaring arrays, where you declare an array of arrays. In essence, this approach is equivalent to declaring a multi-dimensional array in Swag.
*/
#test
{
    var array: [2] [2] s32  // Declare a 2x2 array of s32 using C/C++ syntax
    array[0, 0] = 1
    array[0, 1] = 2
    array[1, 0] = 3
    array[1, 1] = 4
}

/**
The sizes of arrays, including multi-dimensional arrays, can be deduced from the initialization expression.
*/
#test
{
    var array  = [1, 2, 3, 4]  // Size deduced to be 4
    var array1 = [[1, 2], [3, 4]]  // 2x2 array, size deduced from initialization

    #assert @countof(array) == 4  // Verify size is 4
    #assert @countof(array1) == 2  // Verify outer array has 2 elements
}

/**
Swag allows you to initialize an entire array with a single value. This is only available for variables, not constants, and only basic types (integers, floats, strings, bools, runes) are supported.
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
