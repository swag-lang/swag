/**
# Introduction to 'while' Loops
A 'while' loop is a control flow statement that allows repeated execution of a block of code as long as the specified condition evaluates to 'true'. Once the condition becomes 'false', the loop terminates.
*/
#test
{
    var i = 0
    while i < 10 do
        i += 1
    @assert(i == 10)
}

/**
# Breaking Out of a 'while' Loop
The 'break' statement provides a way to exit a 'while' loop before the loop's condition becomes 'false'. This is particularly useful when an early termination of the loop is needed based on a specific condition.
*/
#test
{
    var i = 0
    while i < 10
    {
        if i == 5 do
            break
        i += 1
    }

    @assert(i == 5)
}

/**
# Skipping Iterations with 'continue'
The 'continue' statement allows you to skip the current iteration and proceed directly to the next iteration of the loop. This is useful for ignoring specific conditions within the loop while continuing its execution.
*/
#test
{
    var sum = 0
    var i   = 0
    while i < 10
    {
        i += 1
        if i % 2 == 0 do
            continue
        sum += i
    }

    @assert(sum == 25)
}

/**
# Nested 'while' Loops
A 'while' loop can contain another 'while' loop, forming a nested loop structure. In such cases, the 'break' and 'continue' statements apply only to the loop in which they are directly placed.
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
                break
            count += 1
            j += 1
        }
        i += 1
    }

    @assert(count == 6)
}

/**
# Using 'while' with Complex Conditions
The condition in a 'while' loop can involve complex logical expressions, allowing for more sophisticated and controlled execution of the loop.
*/
#test
{
    var a          = 0
    var b          = 1
    var iterations = 0

    while a < 100 and b < 200
    {
        a += 10
        b += 20
        iterations += 1
    }

    @assert(a == 100)
    @assert(b == 201)
    @assert(iterations == 10)
}
