/**
Remember that dynamic arrays are part of the `Std.Core` module. Here we are only talking about native static arrays.
*/

/**
A static array is declared with `[N]` followed by the type, where `N` is the dimension.
*/
#test
{
    {
        var array: [2] s32 // Static array of two s32
        array[0] = 1
        array[1] = 2
    }
}

/**
You can get the number of elements of an array with `@countof`, and the size in bytes with `@sizeof`.
*/
#test
{
    var array: [2] s32
    #assert @countof(array) == 2
    #assert @sizeof(array) == 2 * @sizeof(s32)
}

/**
You can get the address of the array with `@dataof`.
*/
#test
{
    var array: [2] s32
    var ptr0   = @dataof(array)
    ptr0[0] = 1

    // This is equivalent of taking the address of the first element
    var ptr1 = &array[0]
    ptr1[1] = 2

    @assert(array[0] == 1)
    @assert(array[1] == 2)
}

/**
An **array literal** has the form `[A, B, ...]`.
*/
#test
{
    var arr = [1, 2, 3, 4]
    // An array of four s32
    #assert @countof(arr) == 4
    #assert @typeof(arr) == #type [4] s32
}

/**
The size of the array can be deduced from the initialisation expression.
*/
#test
{
    // Here the dimension is not specified, but as the array is initialized with 2 elements, it can be deduced.
    var array: [] s32 = [1, 2]
    @assert(array[0] == 1)
    @assert(array[1] == 2)
    #assert @countof(array) == 2

    // Here both dimensions and types are deduced thanks to the initialization expression.
    var array1 = ["10", "20", "30"]
    @assert(array1[0] == "10")
    @assert(array1[1] == "20")
    @assert(array1[2] == "30")
    #assert @countof(array1) == 3
}

/**
Like every other types, an array is initialized by default to 0
*/
#test
{
    var array: [2] s32
    @assert(array[0] == 0)
    @assert(array[1] == 0)
}

/**
But for speed, you can force the array to be not initialized with `undefined`.
*/
#test
{
    var array: [100] s32 = undefined
}

/**
A static array (with compile time values) can be stored as a constant.
*/
#test
{
    const array = [1, 2, 3, 4]
    #assert array[0] == 1 // Dereference is done at compile time
    #assert array[3] == 4
}

/**
If the type of the array is not specified, the type of the **first** literal value will be used for all other members.
*/
#test
{
    var arr = [1'f64, 2, 3, 4]
    // Every values are forced to be 'f64'

    #assert @countof(arr) == 4
    #assert @typeof(arr) == #type [4] f64
    @assert(arr[3] == 4.0)
}

/**
Of course an array can have multiple dimensions.
*/

/**
Syntax is `[X, Y, Z...]` where `X`, `Y` and `Z` are dimensions.
*/
#test
{
    var array: [2, 2] s32 // Declare a 2x2 array
    array[0, 0] = 1
    array[0, 1] = 2
    array[1, 0] = 3
    array[1, 1] = 4
}

/**
But the C/C++ syntax is also accepted. You will then declare an array of array instead of an array with multiple dimensions, which in fact is the same...
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
The sizes can be deduced from the initialization expression too.
*/
#test
{
    var array  = [1, 2, 3, 4]
    var array1 = [[1, 2], [3, 4]]

    #assert @countof(array) == 4
    #assert @countof(array1) == 2
}

/**
You can initialize a whole array variable (but **not a constant**) with one single value. Only basic types are accepted (integers, float, string, bool, rune).
*/
#test
{
    // The whole array is initialized with 'true'
    var arr: [2, 2] bool = true
    @assert(arr[0, 0] == true)
    @assert(arr[1, 1] == true)

    // The whole array is initialized with 'string'
    var arr1: [5, 10] string = "string"
    @assert(arr1[0, 0] == "string")
    @assert(arr1[4, 9] == "string")
}
