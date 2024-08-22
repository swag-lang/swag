/**
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
You can use `break` to exit a `while` loop prematurely and `continue` to skip the rest of the current iteration and move on to the next iteration.
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
