/**
`visit` is used to iterate over all elements of a collection. It provides a simple and efficient way to process each item in the collection, whether it's an array, slice, or string.
*/
#test
{
    // Here we visit every byte of the string.
    // At each iteration, the byte will be stored in the variable 'value'.
    visit value in "ABC"
    {
        // '#index' is also available. It stores the loop index.
        let a = #index
        switch a
        {
        case 0:
            @assert(value == `A`)
        case 1:
            @assert(value == `B`)
        case 2:
            @assert(value == `C`)
        }
    }
}

/**
You can name both the **value** and the loop **index**, in that order. This allows for more readable code, especially when working with nested loops or complex data structures.
*/
#test
{
    visit value, index in "ABC"
    {
        let a = index
        switch a
        {
        case 0:
            @assert(value == `A`)
        case 1:
            @assert(value == `B`)
        case 2:
            @assert(value == `C`)
        }
    }
}

/**
Both names are optional. If you don't specify names for the value and index, you can use the default aliases `#alias0` for the value and `#alias1` for the index.
*/
#test
{
    visit "ABC"
    {
        let a = #alias1 // This is the index.
        @assert(a == #index)
        switch a
        {
        case 0:
            @assert(#alias0 == `A`)
        case 1:
            @assert(#alias0 == `B`)
        case 2:
            @assert(#alias0 == `C`)
        }
    }
}

/**
You can visit elements in reverse order by adding the `back` modifier. This is useful when you need to process a collection from end to start.
*/
#test
{
    // Here we visit every byte of the string in reverse order.
    var cpt = 0
    visit #back value in "ABC"
    {
        // '#index' is also available. It stores the loop index.
        switch cpt
        {
        case 0:
            @assert(value == `C`)
            @assert(#index == 2)
        case 1:
            @assert(value == `B`)
            @assert(#index == 1)
        case 2:
            @assert(value == `A`)
            @assert(#index == 0)
        }

        cpt += 1
    }
}

/**
You can visit arrays or slices, which allows you to easily process each element.
*/
#test
{
    var array = [10, 20, 30]

    var result = 0
    visit it in array:
        result += it

    @assert(result == 10 + 20 + 30)
}

/**
`visit` also works for multi-dimensional arrays, making it easy to process complex data structures.
*/
#test
{
    var array: [2, 2] s32 = [[10, 20], [30, 40]]

    var result = 0
    visit it in array:
        result += it

    @assert(result == 10 + 20 + 30 + 40)
}

/**
You can visit elements using a pointer to the value by adding `&` before the value name. This allows you to modify the elements in place.
*/
#test
{
    var array: [2, 2] s32 = [[1, 2], [3, 4]]

    var result = 0
    visit &it in array
    {
        result += dref it
        dref it = 555
    }

    @assert(result == 1 + 2 + 3 + 4)

    @assert(array[0, 0] == 555)
    @assert(array[0, 1] == 555)
    @assert(array[1, 0] == 555)
    @assert(array[1, 1] == 555)
}

/**
# where
You can add a `where` clause after the `visit` to filter the elements you want to process. This is useful for applying conditions directly within the loop.
*/
#test
{
    var array: [] s32 = [1, 2, 3, 4]
    var result = 0

    // Just process even values.
    visit value in array where value & 1 == 0:
        result += value

    @assert(result == 6)

    // Equivalent using an if statement inside the visit loop:
    result = 0
    visit value in array:
        if value & 1 == 0:
            result += value  
    @assert(result == 6) 

    // Equivalent using continue to skip odd values:
    result = 0
    visit value in array
    {
        if (value & 1) != 0:
            continue
        result += value        
    }
    @assert(result == 6)
}
