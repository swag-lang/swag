/**
# Introduction to `visit`
The `visit` statement is designed to iterate over all elements within a collection. It provides a streamlined 
and efficient way to process each item in a collection, which can be an array, slice, or string.
*/

#test
{
    // Iterating through each byte in the string "ABC".
    // The current byte will be stored in the variable 'value'.
    visit value in "ABC"
    {
        // '#index' is available to store the loop index.
        let a = #index                                     // Index of the current iteration
        switch a
        {
        case 0:
            @assert(value == `A`)                          // Check if the first value is 'A'
        case 1:
            @assert(value == `B`)                          // Check if the second value is 'B'
        case 2:
            @assert(value == `C`)                          // Check if the third value is 'C'
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
    visit value, index in "ABC"
    {
        let a = index                                      // The current loop index
        switch a
        {
        case 0:
            @assert(value == `A`)                          // Check if the first value is 'A'
        case 1:
            @assert(value == `B`)                          // Check if the second value is 'B'
        case 2:
            @assert(value == `C`)                          // Check if the third value is 'C'
        }
    }
}

/**
# Using Default Aliases
Both the **value** and the **index** are optional. If names are not explicitly provided, 
the default aliases `#alias0` for the value and `#alias1` for the index can be used.
*/

#test
{
    visit "ABC"
    {
        let a = #alias1                                    // Default alias for the index
        @assert(a == #index)                               // Ensure alias matches the index
        switch a
        {
        case 0:
            @assert(#alias0 == `A`)                        // Check if the first alias is 'A'
        case 1:
            @assert(#alias0 == `B`)                        // Check if the second alias is 'B'
        case 2:
            @assert(#alias0 == `C`)                        // Check if the third alias is 'C'
        }
    }
}

/**
# Reverse Order with `back`
To iterate over elements in reverse order, use the `back` modifier. This is particularly useful 
when processing a collection from the last element to the first.
*/

#test
{
    // Visiting each byte in the string "ABC" in reverse order.
    var cpt = 0
    visit #back value in "ABC"
    {
        // '#index' still stores the loop index, even in reverse order.
        switch cpt
        {
        case 0:
            @assert(value == `C`)                          // First value should be 'C'
            @assert(#index == 2)                           // Index should be 2
        case 1:
            @assert(value == `B`)                          // Second value should be 'B'
            @assert(#index == 1)                           // Index should be 1
        case 2:
            @assert(value == `A`)                          // Third value should be 'A'
            @assert(#index == 0)                           // Index should be 0
        }

        cpt += 1                                           // Increment the counter
    }
}

/**
# Visiting Arrays and Slices
The `visit` statement can be used to iterate over arrays or slices, 
enabling straightforward processing of each element.
*/

#test
{
    var array = [10, 20, 30]                               // Define an array of integers

    var result = 0
    visit it in array:                                     // Iterate over each element in the array
        result += it                                       // Accumulate the values

    @assert(result == 10 + 20 + 30)                        // Ensure the sum matches the expected result
}

/**
# Multi-dimensional Arrays
`visit` supports multi-dimensional arrays, facilitating the processing of complex data structures.
*/

#test
{
    var array: [2, 2] s32 = [[10, 20], [30, 40]]           // Define a 2x2 array

    var result = 0
    visit it in array:                                     // Iterate over each element in the 2D array
        result += it                                       // Accumulate the values

    @assert(result == 10 + 20 + 30 + 40)                   // Ensure the sum matches the expected result
}

/**
# Modifying Elements with `&`
By prefixing the value name with `&`, `visit` allows you to visit elements by reference, 
enabling in-place modification of the elements.
*/

#test
{
    var array: [2, 2] s32 = [[1, 2], [3, 4]]               // Define a 2x2 array

    var result = 0
    visit &it in array
    {
        result += dref it                                  // Accumulate the values
        dref it = 555                                      // Modify each element in place
    }

    @assert(result == 1 + 2 + 3 + 4)                       // Ensure the sum matches the expected result

    @assert(array[0, 0] == 555)                            // Verify the first element is modified
    @assert(array[0, 1] == 555)                            // Verify the second element is modified
    @assert(array[1, 0] == 555)                            // Verify the third element is modified
    @assert(array[1, 1] == 555)                            // Verify the fourth element is modified
}

/**
# Filtering with `where`
The `where` clause can be used with `visit` to filter the elements processed based on specific conditions.
This approach is efficient for conditionally applying logic to only the elements that meet certain criteria.
*/

#test
{
    var array: [] s32 = [1, 2, 3, 4]                       // Define an array of integers
    var result = 0

    // Process only even values using `where`.
    visit value in array where value & 1 == 0:
        result += value                                    // Accumulate only even values

    @assert(result == 6)                                   // Ensure the sum of even values is correct

    // Equivalent using an if statement inside the `visit` loop:
    result = 0
    visit value in array:
        if value & 1 == 0:                                 // Check if the value is even
            result += value                                // Accumulate even values
    @assert(result == 6)                                   // Ensure the sum of even values is correct

    // Equivalent using `continue` to skip odd values:
    result = 0
    visit value in array
    {
        if (value & 1) != 0:                               // Skip odd values
            continue
        result += value                                    // Accumulate even values
    }
    @assert(result == 6)                                   // Ensure the sum of even values is correct
}
