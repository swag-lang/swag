#test
{
    // An array is declared with []
    {
        var array: [2] s32
        array[0] = 1
        array[1] = 2
    }

    // You can get the number of elements of an array with '@countof'
    {
        var array: [2] s32
        @assert(@countof(array) == 2)
        @assert(@sizeof(array) == 2 * @sizeof(s32))
    }

    // You can get the address of the array with '@dataof'
    {
        var array: [2] s32
        ptr := @dataof(array)
        ptr[0] = 1
        ptr[1] = 2

        @assert(array[0] == 1)
        @assert(array[1] == 2)
    }

    // An array literal has the form @[...]
    {
        arr := @[1, 2, 3, 4]
        @assert(@countof(arr) == 4)
        @assert(@typeof(arr) == [4] s32)
    }

    // Size can be deduced from initialisation
    {
        var array: [] s32 = @[1, 2]
        @assert(array[0] == 1)
        @assert(array[1] == 2)

        array1 := @["10", "20", "30"]
        @assert(array1[0] == "10")
        @assert(array1[1] == "20")
        @assert(array1[2] == "30")
    }

    // Like every other types, array is initialized by default to 0
    {
        var array: [2] s32
        @assert(array[0] == 0)
        @assert(array[1] == 0)
    }

    // For speed, you can force the array not to be initialized with '?'
    {
        var array: [100] s32 = ?
    }

    // An array can have multiple dimensions
    {
        var array: [2, 2] s32
        array[0, 0] = 1
        array[0, 1] = 2
        array[1, 0] = 3
        array[1, 1] = 4
    }

    // Sizes can be deduced from initialization too
    {
        array := @[1, 2, 3, 4]
        array1 := @[[1, 2], [3, 4]]
    }

    // An array can be stored as a constant
    {
        const array = @[1, 2, 3, 4]
        #assert(array[0] == 1)
        #assert(array[3] == 4)
    }

    // If type is not specified, the type of the first literal value will be used
    // for all other members
    {
        arr := @[1'f64, 2, 3, 4]
        @assert(@countof(arr) == 4)
        @assert(@typeof(arr) == [4] f64)
        @assert(arr[3] == 4.0)
    }
}