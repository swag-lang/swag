#test
{
    // `visit` is used to visit all the elements of a collection.
    {
        // Here we visit every bytes of the string.
        // At each iteration, the byte will be stored in the variable 'value'
        visit value: "ABC"
        {
            // '@index' is also available. It stores the loop index.
            a := @index
            switch a
            {
            case 0: @assert(value == "A"'u8)
            case 1: @assert(value == "B"'u8)
            case 2: @assert(value == "C"'u8)
            }
        }
    }

    // You can name both the **value** and the loop **index**, in that order.
    {
        visit value, index: "ABC"
        {
            a := index
            switch a
            {
            case 0: @assert(value == "A"'u8)
            case 1: @assert(value == "B"'u8)
            case 2: @assert(value == "C"'u8)
            }
        }
    }

    // Both names are optional. In that case, you can use `@alias0` and `@alias1`. `@alias0` for the value, and `@alias1` for the index.
    {
        visit "ABC"
        {
            a := @alias1    // This is the index
            @assert(a == @index)
            switch a
            {
            case 0: @assert(@alias0 == "A"'u8)
            case 1: @assert(@alias0 == "B"'u8)
            case 2: @assert(@alias0 == "C"'u8)
            }
        }
    }

    // You can visit arrays or slices.
    {
        array := [10, 20, 30]

        result := 0
        visit it: array
            result += it

        @assert(result == 10+20+30)
    }

    // Works also for multi dimensional arrays.
    {
        var array: [2, 2] s32 = [[10, 20], [30, 40]]

        result := 0
        visit it: array
            result += it

        @assert(result == 10+20+30+40)
    }

    // You can visit with a pointer to the value, and not the value itself, by adding `&` before the value name.
    {
        var array: [2, 2] s32 = [[1, 2], [3, 4]]

        result := 0
        visit &it: array
        {
            result += dref it
            dref it = 555
        }

        @assert(result == 1+2+3+4)

        @assert(array[0, 0] == 555)
        @assert(array[0, 1] == 555)
        @assert(array[1, 0] == 555)
        @assert(array[1, 1] == 555)
    }
}
