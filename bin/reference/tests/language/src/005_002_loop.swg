/**
# Introduction to `loop`
The `loop` construct in Swag is a powerful tool for iteration, allowing developers to repeat a block of code a specified number of times. This guide provides an in-depth exploration of the `loop` construct, covering its various features, including basic usage, indexing, naming, reverse loops, early exits, and advanced filtering with the `where` clause.
*/

/**
# Basic Usage
The `loop` expression dictates the number of iterations and is evaluated only **once** before the loop begins. This value must be a **positive integer**.
*/
#test
{
    var cpt = 0
    loop 10:                         // Executes the loop 10 times
        cpt += 1                     // Increment the counter on each iteration
    @assert(cpt == 10)               // Assert that the loop executed exactly 10 times
}

/**
# Using `#index`
Within a loop, the compiler automatically provides the `#index` keyword, which holds the current iteration index, starting from 0.
*/
#test
{
    var cpt = 0'u64
    loop 5                            // Loop 5 times
    {
        cpt += #index                 // Add the current index value to 'cpt' in each iteration
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4) // Assert that 'cpt' equals the sum of the indices
}

/**
# Naming the Loop Index
Assigning a name to the loop index can improve code readability and clarify the loop's intent.
*/
#test
{
    var cpt  = 0
    var cpt1 = 0

    loop i in 5                       // The loop index is named 'i'
    {
        cpt += i                      // Use the named index 'i'
        cpt1 += #index                // '#index' is still accessible and returns the same value as 'i'
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4) // Confirm that 'cpt' equals the sum of indices
    @assert(cpt1 == cpt)              // Confirm that 'cpt1' and 'cpt' are equal
}

/**
# Looping Over Arrays and Slices
The `loop` construct is versatile and can iterate over any collection type that supports the `@countof` intrinsic, such as arrays, slices, and strings.
*/
#test
{
    var arr = [10, 20, 30, 40]        // Define an array with 4 elements
    #assert @countof(arr) == 4        // Verify the array has 4 elements

    var cpt = 0
    loop arr:                         // Loop over the array's elements
        cpt += arr[#index]            // Add the current element's value to 'cpt'
    @assert(cpt == 10 + 20 + 30 + 40) // Verify that 'cpt' equals the sum of the array elements
}

/**
> WARNING:
> When looping over a string, the loop will iterate over each byte, **not** over runes. For handling runes (characters that may be encoded in multiple bytes), consider using the Std.Core module.
*/
#test
{
    var cpt = 0
    loop "⻘":                        // Loop over each byte in the string "⻘"
        cpt += 1                     // Increment the counter for each byte

    @assert(cpt == 3)                // Assert that the character '⻘' consists of 3 bytes
}

/**
# Reverse Looping
To iterate in reverse order, append the `#back` modifier to the `loop` statement.
*/
#test
{
    var cpt = 0

    // Loop in reverse order, starting from index 2 down to 0
    loop #back 3
    {
        if cpt == 0:
            @assert(#index == 2)     // First iteration, index should be 2
        elif cpt == 1:
            @assert(#index == 1)     // Second iteration, index should be 1
        elif cpt == 2:
            @assert(#index == 0)     // Third iteration, index should be 0

        cpt += 1
    }
}

/**
# Break and Continue
The `break` and `continue` keywords provide control over the loop's execution flow. `break` exits the loop early, while `continue` skips the remainder of the current iteration and proceeds with the next.
*/

/**
# Exiting Early with `break`
The `break` keyword allows you to exit the loop before completing all iterations, useful for optimizing performance or when a specific condition is met.
*/
#test
{
    var cpt = 0
    loop x in 10                     // Loop 10 times with index named 'x'
    {
        if x == 5:
            break                    // Exit the loop when 'x' equals 5
        cpt += 1
    }

    @assert(cpt == 5)                // Confirm that the loop executed 5 times
}

/**
# Skipping Iterations with `continue`
The `continue` keyword skips the rest of the current loop iteration and jumps to the next, which is useful when certain conditions should bypass processing.
*/
#test
{
    var cpt = 0
    loop x in 10                     // Loop 10 times with index named 'x'
    {
        if x == 5:
            continue                 // Skip the iteration when 'x' equals 5
        cpt += 1
    }

    @assert(cpt == 9)                // Confirm that the loop executed 9 times, skipping the 5th iteration
}

/**
# Ranges
The `loop` construct supports iteration over a **range** of signed values, enabling flexible iteration over specified intervals.
*/

/**
# Looping Over a Range with `to`
The `to` keyword defines a loop that iterates from one value to another, inclusive. The start value must be less than or equal to the end value.
*/
#test
{
    var count = 0
    var sum   = 0
    loop i in -1 to 1                // Loop from -1 to 1, inclusive
    {
        count += 1                   // Count the number of iterations
        sum += i                     // Sum the current index value
    }

    @assert(sum == 0)                // Verify that the sum is 0 (-1 + 0 + 1)
    @assert(count == 3)              // Confirm that the loop executed 3 times
}

/**
# Excluding the Last Value with `until`
The `until` keyword enables iteration up to, but not including, the end value.
*/
#test
{
    var cpt = 0
    loop i in 1 until 3              // Loop from 1 up to, but excluding, 3
    {
        cpt += i                     // Add the current index value to 'cpt'
    }

    @assert(cpt == 1 + 2)            // Verify that 'cpt' equals the sum of 1 and 2
}

/**
# Reverse Range Looping
When using ranges, you can iterate in reverse order by adding the `#back` modifier after the loop statement.
*/
#test
{
    loop #back 0 to 5                // Loop from 5 down to 0, inclusive
    {
    }

    loop #back -1 to 1               // Loop from 1 down to -1, inclusive
    {
    }

    loop #back -2 until 2            // Loop from 1 down to -2, excluding 2
    {
    }
}

/**
# Infinite Loop
A `loop` without an expression but with a block of code creates an infinite loop, functionally equivalent to `while true {}`. Infinite loops are often controlled with `break` statements.
*/
#test
{
    loop
    {
        if #index == 4:              // Use `#index` to break the loop after 4 iterations
            break
    }
}

/**
# Using `where` Clause
The `where` clause provides conditional filtering within a loop, allowing specific iterations to execute based on defined criteria.
*/

/**
# Basic `where` Clause
The `where` clause is appended directly after the `loop` statement, applying a condition to the loop's index or value. Only iterations that satisfy this condition are executed.
*/
#test
{
    var result = 0

    // Loop over the range from 0 to 9, but only process even indices.
    loop i in 10 where i % 2 == 0
    {
        result += i                  // Sum only even indices
    }

    @assert(result == 0 + 2 + 4 + 6 + 8) // Verify that 'result' equals the sum of even indices
}

/**
# `where` with Arrays
When looping over arrays, the `where` clause can filter elements based on their value or index, enabling targeted iteration.
*/
#test
{
    var arr = [10, 21, 30, 41, 50]
    var sumOfEvens = 0

    // Loop over the array, summing only the even numbers.
    loop i in arr where arr[i] % 2 == 0
    {
        sumOfEvens += arr[i]          // Add the even element values to 'sumOfEvens'
    }

    @assert(sumOfEvens == 10 + 30 + 50) // Verify that 'sumOfEvens' equals the sum of even numbers
}

/**
# Complex Conditions with `where`
The `where` clause supports combining multiple logical expressions, allowing for complex filtering conditions directly within the loop.
*/
#test
{
    var arr = [10, 15, 20, 25, 30, 35]
    var filteredSum = 0

    // Loop over the array, summing only even numbers greater than 15.
    loop i in arr where arr[i] % 2 == 0 and arr[i] > 15
    {
        filteredSum += arr[i]         // Add the element values that meet the condition
    }

    @assert(filteredSum == 20 + 30)   // Verify that 'filteredSum' equals the sum of 20 and 30
}

/**
# `where` with Ranges
The `where` clause can also be applied to loops over ranges, providing precise control over which range values are processed in the loop.
*/
#test
{
    var sumOfPositiveEvens = 0

    // Loop over the range from -5 to 5, but process only positive even numbers.
    loop i in -5 to 5 where i > 0 and i % 2 == 0
    {
        sumOfPositiveEvens += i       // Add the positive even values to 'sumOfPositiveEvens'
    }

    @assert(sumOfPositiveEvens == 2 + 4) // Verify that 'sumOfPositiveEvens' equals the sum of 2 and 4
}

/**
# Combining `back` and `where`
You can combine the `#back` modifier with the `where` clause to filter values while iterating in reverse order.
*/
#test
{
    var arr = [10, 20, 30, 40, 50]
    var reversedSum = 0

    // Loop through the array in reverse order, summing only even values.
    loop #back i in arr where arr[i] % 2 == 0
    {
        reversedSum += arr[i]         // Add the even values to 'reversedSum'
    }

    @assert(reversedSum == 50 + 40 + 30 + 20 + 10) // Verify the sum of even values in reverse
}

/**
# Complex Logical Expressions with `where`
The `where` clause in Swag allows for complex logical expressions, enabling intricate filtering criteria directly within the loop construct.
*/
#test
{
    var arr = [10, 25, 30, 45, 50, 65]
    var complexSum = 0

    // Loop over the array, summing elements that are either even or greater than 40.
    loop i in arr where arr[i] % 2 == 0 or arr[i] > 40
    {
        complexSum += arr[i]          // Add the values that meet the complex condition to 'complexSum'
    }

    @assert(complexSum == 10 + 30 + 45 + 50 + 65) // Verify that 'complexSum' equals the sum of matching values
}
