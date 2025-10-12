/**
# Introduction to 'while' Loops

A `while` loop repeatedly executes a block of code as long as its condition evaluates to `true`.
Once the condition becomes `false`, the loop terminates.
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

The `break` statement allows an early exit from a `while` loop before the condition becomes `false`.
This is useful when you need to stop the loop based on a specific condition.
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

The `continue` statement skips the current iteration and jumps to the next one.
It is useful for ignoring specific cases within the loop.
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

A `while` loop can contain another `while` loop.
`break` and `continue` only affect the loop in which they are directly used.
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

The condition in a `while` loop can include logical operators such as `and` and `or`
for more complex and controlled iteration logic.
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
