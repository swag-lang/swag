/**
# Introduction to 'foreach'
The 'foreach' statement iterates over all elements of a collection (arrays, slices, strings, and even structs),
providing a streamlined and efficient way to process each item.
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
You can explicitly name both the element 'value' and the loop 'index' to improve readability,
especially in nested loops or with complex data structures.
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
If you do not name the element or index, you can access them with the default aliases:
'#alias0' for the element and '#alias1' for the index.
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
Iterate from the last element to the first by adding the '#reverse' modifier.
'@index' still reflects the loop index within the original collection.
*/

#test
{
    var cpt = 0
    foreach #reverse value in "ABC"
    {
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
Use 'foreach' to traverse arrays or slices and process each element.
*/

#test
{
    var array  = [10, 20, 30]
    var result = 0
    foreach it in array do
        result += it
    @assert(result == 10 + 20 + 30)
}

/**
# Multi-dimensional Arrays
'foreach' works with multi-dimensional arrays and visits each element in row-major order.
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
Prefix the element name with '&' to visit elements by address and modify them in place.
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
Add a 'where' clause to process only elements that satisfy a condition.
This avoids explicit branching inside the loop.
*/

#test
{
    var array: [?] s32 = [1, 2, 3, 4]
    var result = 0

    foreach value in array where value & 1 == 0 do
        result += value
    @assert(result == 6)

    // Equivalent with an 'if' guard:
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
