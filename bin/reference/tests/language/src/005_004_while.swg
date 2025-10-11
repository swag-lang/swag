/**
# Introduction to `while` Loops
A `while` loop is a control flow statement that allows repeated execution of a block of code as long as the specified condition evaluates to `true`. Once the condition becomes `false`, the loop terminates.
*/
#test
{
    var i = 0
    while i < 10 do  // Loop executes repeatedly until 'i' is no longer less than 10
        i += 1 // Increment 'i' by 1 on each iteration
    @assert(i == 10) // Verify that 'i' equals 10 after loop completion
}

/**
# Breaking Out of a `while` Loop
The `break` statement provides a way to exit a `while` loop before the loop’s condition becomes `false`. This is particularly useful when an early termination of the loop is needed based on a specific condition.
*/
#test
{
    var i = 0
    while i < 10
    {
        if i == 5 do
            break // Exit the loop immediately when 'i' reaches 5
        i += 1 // Increment 'i' by 1 on each iteration
    }

    @assert(i == 5) // Confirm that 'i' equals 5 after breaking out of the loop
}

/**
# Skipping Iterations with `continue`
The `continue` statement allows you to skip the current iteration and proceed directly to the next iteration of the loop. This is useful for ignoring specific conditions within the loop while continuing its execution.
*/
#test
{
    var sum = 0
    var i   = 0
    while i < 10
    {
        i += 1 // Increment 'i' by 1 at the start of each iteration
        if i % 2 == 0 do
            continue // Skip adding 'i' to 'sum' if 'i' is an even number
        sum += i // Add only odd numbers to 'sum'
    }

    @assert(sum == 25) // Ensure that the sum of odd numbers from 1 to 9 equals 25
}

/**
# Nested `while` Loops
A `while` loop can contain another `while` loop, forming a nested loop structure. In such cases, the `break` and `continue` statements apply only to the loop in which they are directly placed.
*/
#test
{
    var i     = 0
    var j     = 0
    var count = 0

    while i < 3
    {
        j = 0
        while j < 3
        {
            if j == 2 do
                break // Exit the inner loop when 'j' equals 2
            count += 1 // Increment 'count' each time the inner loop completes an iteration
            j += 1 // Increment 'j' by 1 on each iteration of the inner loop
        }
        i += 1 // Increment 'i' by 1 on each iteration of the outer loop
    }

    @assert(count == 6) // Confirm that 'count' equals 6, indicating the inner loop ran 6 times
}

/**
# Using `while` with Complex Conditions
The condition in a `while` loop can involve complex logical expressions, allowing for more sophisticated and controlled execution of the loop.
*/
#test
{
    var a          = 0
    var b          = 1
    var iterations = 0

    while a < 100 and b < 200
    {
        a += 10 // Increment 'a' by 10 on each iteration
        b += 20 // Increment 'b' by 20 on each iteration
        iterations += 1 // Increment 'iterations' to track the number of loop executions
    }

    @assert(a == 100) // Ensure that 'a' reaches 100 upon loop completion
    @assert(b == 201) // Ensure that 'b' reaches 201 upon loop completion
    @assert(iterations == 10) // Confirm that the loop executed 10 times
}
