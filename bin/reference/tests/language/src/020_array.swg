// Remember that dynamic arrays are part of the 'Std.Core' module.
// Here we are only talking about native static arrays.

#test
{
    // A static array is declared with [] followed by the type
    {
        var array: [2] s32  // Static array of two s32
        array[0] = 1
        array[1] = 2
    }

    // You can get the number of elements of an array with '@countof'
    {
        var array: [2] s32
        #assert @countof(array) == 2
        #assert @sizeof(array) == 2 * @sizeof(s32)
    }

    // You can get the address of the array with '@dataof'
    {
        var array: [2] s32
        ptr0 := @dataof(array)
        ptr0[0] = 1

        // This is equivalent of taking the address of the first element
        ptr1 := &array[0]
        ptr1[1] = 2

        @assert(array[0] == 1)
        @assert(array[1] == 2)
    }

    // An array literal has the form @[...]
    {
        arr := @[1, 2, 3, 4]
        #assert @countof(arr) == 4
        #assert @typeof(arr) == [4] s32
    }

    // The size of the array can be deduced from the initialisation expression
    {
        var array: [] s32 = @[1, 2]
        @assert(array[0] == 1)
        @assert(array[1] == 2)
        #assert @countof(array) == 2

        array1 := @["10", "20", "30"]
        @assert(array1[0] == "10")
        @assert(array1[1] == "20")
        @assert(array1[2] == "30")
        #assert @countof(array1) == 3
    }

    // Like every other types, an array is initialized by default to 0
    {
        var array: [2] s32
        @assert(array[0] == 0)
        @assert(array[1] == 0)
    }

    // But for speed, you can force the array to be not initialized with '?'
    {
        var array: [100] s32 = ?
    }

    // An array can have multiple dimensions
    // Syntax is *not* [X][Y] like in C/C++, but is [X, Y]
    {
        var array: [2, 2] s32
        array[0, 0] = 1
        array[0, 1] = 2
        array[1, 0] = 3
        array[1, 1] = 4
    }

    // The sizes can be deduced from the initialization expression too
    {
        array  := @[1, 2, 3, 4]
        array1 := @[[1, 2], [3, 4]]

        #assert @countof(array) == 4
        #assert @countof(array) == 4
    }

    // A static array (with compile time values) can be stored as a constant
    {
        const array = @[1, 2, 3, 4]
        #assert array[0] == 1   // Dereference is done at compile time
        #assert array[3] == 4
    }

    // If the type of the array is not specified, the type of the first literal value will be used
    // for all other members
    {
        arr := @[1'f64, 2, 3, 4]    // Every values are forced to be 'f64'

        #assert @countof(arr) == 4
        #assert @typeof(arr) == [4] f64
        @assert(arr[3] == 4.0)
    }
}