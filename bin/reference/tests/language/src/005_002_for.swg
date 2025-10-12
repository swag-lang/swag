/**
# Introduction to 'for'
The 'for' construct in Swag is a tool for iteration, allowing developers to repeat a block of code a specified number of times. This guide provides an in-depth exploration of the 'for' construct, covering its various features, including basic usage, indexing, naming, reverse loops, early exits, and advanced filtering with the 'where' clause.
*/

/**
# Basic Usage
The 'for' expression dictates the number of iterations and is evaluated only **once** before the loop begins. This value must be a **positive integer**.
*/
#test
{
    var cpt = 0
    for 10 do
        cpt += 1
    @assert(cpt == 10)
}

/**
# Using '@index'
Within a 'for', the compiler automatically provides the '@index' keyword, which holds the current iteration index, starting from 0.
*/
#test
{
    var cpt = 0'u64
    for 5
    {
        cpt += @index
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
}

/**
# Naming the Loop Index
Assigning a name to the 'for' index can improve code readability and clarify the for's intent.
*/
#test
{
    var cpt  = 0
    var cpt1 = 0

    for i in 5
    {
        cpt += i
        cpt1 += @index // '@index' is still accessible and returns the same value as 'i'
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
    @assert(cpt1 == cpt)
}

/**
# Looping Over Arrays and Slices
The 'for' construct is versatile and can iterate over any collection type that supports the '@countof' intrinsic, such as arrays, slices, and strings.
*/
#test
{
    var arr = [10, 20, 30, 40]
    #assert(@countof(arr) == 4)

    var cpt = 0
    for arr do
        cpt += arr[@index]
    @assert(cpt == 10 + 20 + 30 + 40)
}

/**
> WARNING:
> When looping over a string, the for will iterate over each byte, **not** over runes. For handling runes (characters that may be encoded in multiple bytes), consider using the Std.Core module.
*/
#test
{
    var cpt = 0
    for "⻘" do
        cpt += 1

    @assert(cpt == 3)
}

/**
# Reverse Looping
To iterate in reverse order, append the '#reverse' modifier to the 'for' statement.
*/
#test
{
    var cpt = 0

    for #reverse 3
    {
        if cpt == 0 do
            @assert(@index == 2)
        elif cpt == 1 do
            @assert(@index == 1)
        elif cpt == 2 do
            @assert(@index == 0)

        cpt += 1
    }
}

/**
# 'break' and 'continue'
The 'break' and 'continue' keywords provide control over the for's execution flow. 'break' exits the loop early, while 'continue' skips the remainder of the current iteration and proceeds with the next.
*/

/**
# Exiting Early with 'break'
The 'break' keyword allows you to exit the loop before completing all iterations, useful for optimizing performance or when a specific condition is met.
*/
#test
{
    var cpt = 0
    for x in 10
    {
        if x == 5 do
            break
        cpt += 1
    }

    @assert(cpt == 5)
}

/**
# Skipping Iterations with 'continue'
The 'continue' keyword skips the rest of the current for iteration and jumps to the next, which is useful when certain conditions should bypass processing.
*/
#test
{
    var cpt = 0
    for x in 10
    {
        if x == 5 do
            continue
        cpt += 1
    }

    @assert(cpt == 9)
}

/**
# Ranges
The 'for' construct supports iteration over a **range** of signed values, enabling flexible iteration over specified intervals.
*/

/**
# Looping Over a Range with 'to'
The 'to' keyword defines a loop that iterates from one value to another, inclusive.

> WARNING:
> The start value must be less than or equal to the end value.
*/
#test
{
    var count = 0
    var sum   = 0
    for i in -1 to 1
    {
        count += 1
        sum += i
    }

    @assert(sum == 0)
    @assert(count == 3)
}

/**
# Excluding the Last Value with 'until'
The 'until' keyword enables iteration up to, but not including, the end value.
*/
#test
{
    var cpt = 0
    for i in 1 until 3
    {
        cpt += i
    }

    @assert(cpt == 1 + 2)
}

/**
# Reverse Range Looping
When using ranges, you can also iterate in reverse order by adding the '#reverse' modifier after the for statement.
*/
#test
{
    for #reverse 0 to 5
    {
    }

    for #reverse -1 to 1
    {
    }

    for #reverse -2 until 2
    {
    }
}

/**
# Infinite Loop
A 'for' without an expression but with a block of code creates an infinite loop, functionally equivalent to 'while true {}'. Infinite loops are often controlled with 'break' statements.
*/
#test
{
    for
    {
        if @index == 4 do
            break
    }
}

/**
# Using 'where' Clause
The 'where' clause provides conditional filtering within a loop, allowing specific iterations to execute based on defined criteria.
*/

/**
# Basic 'where' Clause
The 'where' clause is appended directly after the 'for' statement, applying a condition to the loop's index or value. Only iterations that satisfy this condition are executed.
*/
#test
{
    var result = 0

    for i in 10 where i % 2 == 0
    {
        result += i
    }

    @assert(result == 0 + 2 + 4 + 6 + 8)
}

/**
# 'where' with Arrays
When looping over arrays, the 'where' clause can filter elements based on their value or index, enabling targeted iteration.
*/
#test
{
    var arr        = [10, 21, 30, 41, 50]
    var sumOfEvens = 0

    for i in arr where arr[i] % 2 == 0
    {
        sumOfEvens += arr[i]
    }

    @assert(sumOfEvens == 10 + 30 + 50)
}

/**
# Complex Conditions with 'where'
The 'where' supports combining multiple logical expressions, allowing for complex filtering conditions directly within the for.
*/
#test
{
    var arr         = [10, 15, 20, 25, 30, 35]
    var filteredSum = 0

    for i in arr
        where arr[i] % 2 == 0 and arr[i] > 15
        {
            filteredSum += arr[i]
        }

    @assert(filteredSum == 20 + 30)
}

#test
{
    var arr        = [10, 25, 30, 45, 50, 65]
    var complexSum = 0

    for i in arr where arr[i] % 2 == 0 or arr[i] > 40
    {
        complexSum += arr[i]
    }

    @assert(complexSum == 10 + 30 + 45 + 50 + 65)
}

/**
# 'where' with Ranges
The 'where' clause can also be applied to loops over ranges, providing precise control over which range values are processed in the 'for'.
*/
#test
{
    var sumOfPositiveEvens = 0

    for i in -5 to 5 where i > 0 and i % 2 == 0
    {
        sumOfPositiveEvens += i
    }

    @assert(sumOfPositiveEvens == 2 + 4)
}

/**
# Combining 'back' and 'where'
You can combine the '#reverse' modifier with the 'where' clause to filter values while iterating in reverse order.
*/
#test
{
    var arr         = [10, 20, 30, 40, 50]
    var reversedSum = 0

    for #reverse i in arr where arr[i] % 2 == 0
    {
        reversedSum += arr[i]
    }

    @assert(reversedSum == 50 + 40 + 30 + 20 + 10)
}

/**
# C like for
The `for` loop also offers a versatile way to iterate over a range of values. The structure closely follows that of C/C++ loops, consisting of a *variable declaration statement*, a *test expression*, and an *ending statement*. This provides fine-grained control over the for's execution, making it a powerful tool for various iteration scenarios.
*/
#test
{
    var cpt = 0

    // A standard 'for' loop with initialization, condition, and increment.
    for var i = 0; i < 10; i += 1 do
        cpt += 1
    @assert(cpt == 10)

    // Alternative syntax: Semicolons ';' can be replaced by newlines for cleaner, more readable code.
    for var i = 0; i < 10; i += 1
    {
        cpt += 1
    }
    @assert(cpt == 20)
}

/**
# Accessing Loop Index with `@index`

Similar to other looping constructs like `for`, `foreach`, and `while`, the `for` loop in Swag provides access to the `@index` keyword. This keyword represents the **current for index** and is particularly useful when you need to keep track of the iteration count separately from the for variable.
*/
#test
{
    var cpt = 0'u64

    // Using `@index` to accumulate the for indices.
    for var i: u32 = 10; i < 15; i += 1 do
        cpt += @index
    @assert(cpt == 0 + 1 + 2 + 3 + 4)

    var cpt1 = 0'u64
    for var i = 10; i < 15; i += 1 do
        cpt1 += @index
    @assert(cpt1 == 0 + 1 + 2 + 3 + 4)
}

/**
# Using `break` and `continue` in `for` Loops

In Swag, `break` and `continue` work within `for` loops just as they do in other for structures. Use `break` to exit the loop prematurely, effectively terminating the loop when a specific condition is met. The `continue` statement, on the other hand, skips the remainder of the current for iteration and jumps to the next iteration.
*/
#test
{
    var sum = 0

    for var i = 0; i < 10; i += 1
    {
        if i == 5 do
            break // Exit the for when 'i' equals 5
        sum += i
    }

    @assert(sum == 0 + 1 + 2 + 3 + 4) // Sum is 10

    sum = 0
    for var i = 0; i < 10; i += 1
    {
        if i % 2 == 0 do
            continue // Skip even numbers
        sum += i
    }

    @assert(sum == 1 + 3 + 5 + 7 + 9) // Sum is 25
}

/**
# Nested `for` Loops

Swag supports nested `for` loops, which are useful for more complex iteration patterns. In nested loops, the `@index` keyword refers to the current index of the innermost for.
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
            result += @index // Adds the index of the inner for
        }
    }

    @assert(result == 10 * 5) // Each inner for runs 5 times, so the sum of indices (0+1+2+3+4) * 5 = 10*5
}

/**
# Iterating Over Arrays with `for`

The `for` for can also be used to iterate over elements of an array or other iterable collections. This method provides a straightforward way to process or manipulate each element within a collection (but we'll see later that `foreach` is better).
*/
#test
{
    var array = [1, 2, 3, 4, 5]
    var sum   = 0

    for var i = 0; i < @countof(array); i += 1
    {
        sum += array[i]
    }

    @assert(sum == 1 + 2 + 3 + 4 + 5) // Sum is 15
}
