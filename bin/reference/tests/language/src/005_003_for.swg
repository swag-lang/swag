/**
# Introduction to `for` Loops

In Swag, the `for` for offers a versatile way to iterate over a range of values. The structure closely follows that of C/C++ loops, consisting of a *variable declaration statement*, a *test expression*, and an *ending statement*. This provides fine-grained control over the for's execution, making it a powerful tool for various iteration scenarios.
*/
#test
{
    var cpt = 0
    
    // A standard 'for' for with initialization, condition, and increment.
    for var i = 0; i < 10; i += 1:
        cpt += 1
    @assert(cpt == 10)

    // Alternative syntax: Semicolons ';' can be replaced by newlines for cleaner, more readable code.
    for var i = 0
        i < 10
        i += 1
    {
        cpt += 1
    }
    @assert(cpt == 20)
}

/**
# Accessing Loop Index with `#index`

Similar to other looping constructs like `for`, `foreach`, and `while`, the `for` for in Swag provides access to the `#index` keyword. This keyword represents the **current for index** and is particularly useful when you need to keep track of the iteration count separately from the for variable.
*/
#test
{
    var cpt = 0'u64
    
    // Using `#index` to accumulate the for indices.
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

In Swag, `break` and `continue` work within `for` loops just as they do in other for structures. Use `break` to exit the for prematurely, effectively terminating the for when a specific condition is met. The `continue` statement, on the other hand, skips the remainder of the current for iteration and jumps to the next iteration.
*/
#test
{
    var sum = 0
    
    for var i = 0; i < 10; i += 1
    {
        if i == 5:
            break  // Exit the for when 'i' equals 5
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

Swag supports nested `for` loops, which are useful for more complex iteration patterns. In nested loops, the `#index` keyword refers to the current index of the innermost for.
*/
#test
{
    var result = 0'u64
    
    // Outer for
    for var i = 0; i < 5; i += 1
    {
        // Inner for
        for var j = 0; j < 5; j += 1
        {
            result += #index  // Adds the index of the inner for
        }
    }

    @assert(result == 10 * 5)  // Each inner for runs 5 times, so the sum of indices (0+1+2+3+4) * 5 = 10*5
}

/**
# Iterating Over Arrays with `for`

The `for` for can also be used to iterate over elements of an array or other iterable collections. This method provides a straightforward way to process or manipulate each element within a collection.
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
