/**
# Introduction to `while` Loops
`while` is a loop that continues to execute as long as the specified expression evaluates to `true`. The loop stops running when the expression becomes `false`.
*/
#test
{
    var i = 0
    while i < 10:        // Loop runs until 'i' is no longer less than 10
        i += 1
    @assert(i == 10)     // Asserts that 'i' is 10 after the loop exits
}

/**
# Breaking Out of a `while` Loop
You can use `break` to exit a `while` loop prematurely. This is useful when you want to stop the loop based on a certain condition before the loop's natural termination.
*/
#test
{
    var i = 0
    while i < 10
    {
        if i == 5:
            break        // Exit the loop when 'i' equals 5
        i += 1
    }

    @assert(i == 5)       // Asserts that 'i' equals 5 after breaking out of the loop
}

/**
# Skipping Iterations with `continue`
`continue` can be used to skip the rest of the current iteration and immediately proceed to the next iteration of the loop.
*/
#test
{
    var sum = 0
    var i = 0
    while i < 10
    {
        i += 1
        if i % 2 == 0:
            continue      // Skip the even numbers
        sum += i          // Add only odd numbers to 'sum'
    }

    @assert(sum == 25)    // Asserts that the sum of odd numbers from 1 to 9 is 25
}

/**
# Nested `while` Loops
`while` loops can be nested within each other. The `break` and `continue` statements only affect the innermost loop in which they are placed.
*/
#test
{
    var i = 0
    var j = 0
    var count = 0

    while i < 3
    {
        j = 0
        while j < 3
        {
            if j == 2:
                break      // Breaks out of the inner loop when 'j' equals 2
            count += 1
            j += 1
        }
        i += 1
    }

    @assert(count == 6)    // Asserts that the inner loop ran 6 times (2 times for each of the 3 iterations of the outer loop)
}

/**
# Using `while` with Complex Conditions
The `while` loop condition can involve complex logical expressions, allowing for intricate control flow.
*/
#test
{
    var a = 0
    var b = 1
    var iterations = 0

    while a < 100 and b < 200
    {
        a += 10
        b += 20
        iterations += 1
    }

    @assert(a == 100)         // Asserts that 'a' reached 100
    @assert(b == 201)         // Asserts that 'b' reached 201
    @assert(iterations == 10) // Asserts that the loop ran 5 times
}
