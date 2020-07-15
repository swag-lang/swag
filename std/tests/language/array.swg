#test
{
    // An array is declared with []
    {
        var array: [2] s32
        array[0] = 1
        array[1] = 2
    }

    // Size can be deduced from initialisation
    {
        var array: [] s32 = [1, 2]
        @assert(array[0] == 1)
        @assert(array[1] == 2)

        array1 := ["10", "20", "30"]
        @assert(array1[0] == "10")
        @assert(array1[1] == "20")
        @assert(array1[2] == "30")
    }

    // Like every types, array is initialized to 0
    {
        var array: [2] s32
        @assert(array[0] == 0)
        @assert(array[1] == 0)
    }

    // To optimize, you can force the array not to be initialized
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
        array := [[1, 2], [3, 4]]
    }
}