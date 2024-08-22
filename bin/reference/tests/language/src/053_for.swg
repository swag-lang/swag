/**
# Introduction to `for` Loops
`for` loops in Swag follow the traditional structure seen in C/C++: a *start statement*, a *test expression*, and an *ending statement*. This structure provides a versatile way to iterate over a range of values with fine control over the loop's behavior.
*/
#test
{
    var cpt = 0
    // Standard 'for' loop with initialization, condition, and increment.
    for var i = 0; i < 10; i += 1;:
        cpt += 1
    @assert(cpt == 10)

    // Semicolons ';' can be replaced by newlines for cleaner syntax.
    for var i = 0
        i < 10
        i += 1
    {
        cpt += 1
    }
    @assert(cpt == 20)

    // Instead of a single expression, you can use a statement block for the initialization or increment.
    for { var i = 0; cpt = 0; }
        i < 10
        i += 1
    {
        cpt += 1
    }
    @assert(cpt == 10)

    for { var i = 0; cpt = 0; }
        i < 10
        { i += 2; i -= 1; }
    {
        cpt += 1
    }
    @assert(cpt == 10)
}

/**
# Accessing Loop Index with `#index`
Like `loop`, `visit`, and `while`, the `for` loop gives you access to `#index`, which represents the **current loop index**. This can be useful when you need to keep track of the iteration count separately from the loop variable.
*/
#test
{
    var cpt = 0'u64
    // Use of `#index` to accumulate the loop indices.
    for var i: u32 = 10; i < 15; i += 1:
        cpt += #index
    @assert(cpt == 0+1+2+3+4)

    var cpt1 = 0'u64
    for var i = 10; i < 15; i += 1:
        cpt1 += #index
    @assert(cpt1 == 0+1+2+3+4)
}

/**
# Using `break` and `continue` in `for` Loops
As with other loops, you can use `break` to exit a `for` loop prematurely and `continue` to skip the rest of the current iteration and move on to the next iteration.
*/
#test
{
    var sum = 0
    for var i = 0; i < 10; i += 1
    {
        if i == 5:
            break  // Exit the loop when 'i' equals 5
        sum += i
    }
    @assert(sum == 0+1+2+3+4)  // Sum is 10

    sum = 0
    for var i = 0; i < 10; i += 1
    {
        if i % 2 == 0:
            continue  // Skip even numbers
        sum += i
    }
    @assert(sum == 1+3+5+7+9)  // Sum is 25
}

/**
# Nested `for` Loops
`for` loops can be nested, allowing for more complex iteration patterns. The `#index` within a nested loop corresponds to the innermost loop.
*/
#test
{
    var result = 0'u64
    
    // Outer loop
    for var i = 0; i < 5; i += 1
    {
        // Inner loop
        for var j = 0; j < 5; j += 1
        {
            result += #index  // Adds the index of the inner loop
        }
    }
    @assert(result == 10 * 5)  // Each inner loop runs 5 times, so the sum of indices (0+1+2+3+4) * 5 = 10*5
}

/**
# Iterating Over Arrays with `for`
The `for` loop can also be used to iterate over elements of an array or other iterable collections.
*/
#test
{
    var array = [1, 2, 3, 4, 5]
    var sum = 0

    for var i = 0; i < @countof(array); i += 1
    {
        sum += array[i]
    }
    @assert(sum == 1+2+3+4+5)  // Sum is 15
}
