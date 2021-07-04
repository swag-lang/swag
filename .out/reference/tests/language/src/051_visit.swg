#test
{
    // 'visit' is used to visit all the elements of a collection
    {
        visit value: "ABC"
        {
            a := @index
            switch a
            {
            case 0: @assert(value == "A"'u8)
            case 1: @assert(value == "B"'u8)
            case 2: @assert(value == "C"'u8)
            }
        }
    }

    // You can name both the value and the loop index, in that order
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

    // Both names are optional. In that case, you can use '@alias0' and '@alias1'
    {
        visit "ABC"
        {
            a := @alias1
            @assert(a == @index)
            switch a
            {
            case 0: @assert(@alias0 == "A"'u8)
            case 1: @assert(@alias0 == "B"'u8)
            case 2: @assert(@alias0 == "C"'u8)
            }
        }
    }

    // You can visit arrays or slices
    {
        array := @[10, 20, 30]

        result := 0
        visit it: array
        {
            result += it
        }

        @assert(result == 10+20+30)
    }

    // Works also for multi dimensional arrays
    {
        var array: [2, 2] s32 = @[[1, 2], [3, 4]]

        result := 0
        visit it: array result += it
        @assert(result == 1+2+3+4)
    }

    // You can visit with a pointer to the value, and not the value itself, by
    // adding '*' before the value name
    {
        var array: [2, 2] s32 = @[[1, 2], [3, 4]]

        result := 0
        visit *it: array
        {
            result += dref it
        }

        @assert(result == 1+2+3+4)
    }
}