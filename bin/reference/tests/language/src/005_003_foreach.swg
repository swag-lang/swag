/**
# Introduction to 'foreach'
The 'foreach' statement is designed to iterate over all elements within a collection. It provides a streamlined
and efficient way to process each item in a collection, which can be an array, slice, string and even a struct.
*/

#test
{
    foreach value in "ABC"
    {
        let a = @index
        switch a
        {
        case 0:
            @assert(value == 'A')
        case 1:
            @assert(value == 'B')
        case 2:
            @assert(value == 'C')
        }
    }
}

/**
# Naming the Value and Index
Both the **value** and the loop **index** can be named explicitly. This enhances code readability,
especially in cases involving nested loops or complex data structures.
*/

#test
{
    foreach value, index in "ABC"
    {
        let a = index
        switch a
        {
        case 0:
            @assert(value == 'A')
        case 1:
            @assert(value == 'B')
        case 2:
            @assert(value == 'C')
        }
    }
}

/**
# Using Default Aliases
Both the **value** and the **index** are optional. If names are not explicitly provided,
the default aliases '#alias0' for the value and '#alias1' for the index can be used.
*/

#test
{
    foreach "ABC"
    {
        let a = #alias1
        @assert(a == @index)
        switch a
        {
        case 0:
            @assert(#alias0 == 'A')
        case 1:
            @assert(#alias0 == 'B')
        case 2:
            @assert(#alias0 == 'C')
        }
    }
}

/**
# Reverse Order with '#reverse'
To iterate over elements in reverse order, use the '#reverse' modifier. This is particularly useful
when processing a collection from the last element to the first.
*/

#test
{
    var cpt = 0
    foreach #reverse value in "ABC"
    {
        // '@index' still stores the loop index, even in reverse order.
        switch cpt
        {
        case 0:
            @assert(value == 'C')
            @assert(@index == 2)
        case 1:
            @assert(value == 'B')
            @assert(@index == 1)
        case 2:
            @assert(value == 'A')
            @assert(@index == 0)
        }

        cpt += 1
    }
}

/**
# Visiting Arrays and Slices
The 'foreach' statement can be used to iterate over arrays or slices,
enabling straightforward processing of each element.
*/

#test
{
    var array = [10, 20, 30]

    var result = 0
    foreach it in array do
        result += it

    @assert(result == 10 + 20 + 30)
}

/**
# Multi-dimensional Arrays
'foreach' supports multi-dimensional arrays, facilitating the processing of complex data structures.
*/

#test
{
    var array: [2, 2] s32 = [[10, 20], [30, 40]]

    var result = 0
    foreach it in array do
        result += it

    @assert(result == 10 + 20 + 30 + 40)
}

/**
# Modifying Elements with '&'
By prefixing the value name with '&', 'foreach' allows you to visit elements by address,
enabling in-place modification of the elements.
*/

#test
{
    var array: [2, 2] s32 = [[1, 2], [3, 4]]

    var result = 0
    foreach &it in array
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
# Filtering with 'where'
The 'where' clause can be used with 'foreach' to filter the elements processed based on specific conditions.
This approach is efficient for conditionally applying logic to only the elements that meet certain criteria.
*/

#test
{
    var array: [?] s32 = [1, 2, 3, 4]
    var result = 0

    foreach value in array where value & 1 == 0 do
        result += value

    @assert(result == 6)

    // Equivalent using an if statement inside the 'foreach' loop:
    result = 0
    foreach value in array do
        if value & 1 == 0 do
            result += value
    @assert(result == 6)

    // Equivalent using 'continue' to skip odd values:
    result = 0
    foreach value in array
    {
        if (value & 1) != 0 do
            continue
        result += value
    }
    @assert(result == 6)
}
