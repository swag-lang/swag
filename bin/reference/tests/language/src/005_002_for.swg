/**
# Introduction to 'for'

The `for` construct in Swag enables iteration, allowing a block of code to execute repeatedly.
This guide explores its features: basic usage, indexing, naming, reverse loops, early exits,
range iteration, and advanced filtering using the `where` clause.
*/

/**
# Basic Usage

The `for` expression specifies the number of iterations and is evaluated **once** before the loop starts.
This expression must yield a **positive integer**.
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

Within a `for`, the compiler provides the built-in `@index`, representing the current iteration (starting from 0).
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

You can assign a custom name to the loop index for clarity, while `@index` remains accessible.
*/
#test
{
    var cpt  = 0
    var cpt1 = 0

    for i in 5
    {
        cpt += i
        cpt1 += @index
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
    @assert(cpt1 == cpt)
}

/**
# Looping Over Arrays and Slices

The `for` construct can iterate over any type supporting `@countof`, such as arrays, slices, or strings.
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
> When iterating over strings, `for` loops over **bytes**, not runes.
> Use `Std.Core` for rune-based iteration.
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

To iterate in reverse order, add the `#reverse` modifier.
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

The `break` and `continue` keywords control loop flow.
`break` exits the loop, while `continue` skips to the next iteration.
*/

/**
# Exiting Early with 'break'

`break` stops the loop before completing all iterations.
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

`continue` skips the current iteration and proceeds to the next.
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

The `for` loop supports signed integer ranges, offering flexible intervals.
*/

/**
# Looping Over a Range with 'to'

`to` defines an inclusive range loop.

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

`until` defines a range that excludes the end value.
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

Use `#reverse` to iterate a range in reverse order.
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

A `for` without an expression creates an infinite loop, similar to `while true {}`.
Use `break` to exit.
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

The `where` clause filters iterations based on conditions.
*/

/**
# Basic 'where' Clause

Attach `where` to apply a filter to the loop index or element.
Only iterations meeting the condition are executed.
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

Use `where` to filter array elements during iteration.
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

The `where` clause supports multiple logical conditions for advanced filtering.
*/
#test
{
    var arr         = [10, 15, 20, 25, 30, 35]
    var filteredSum = 0

    for i in arr where arr[i] % 2 == 0 and arr[i] > 15
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

The `where` clause can also be applied to ranges.
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
# Combining '#reverse' and 'where'

You can combine `#reverse` with `where` for reverse conditional iteration.
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
# C-like 'for'

Swag also supports a C-style `for` loop with initialization, condition, and increment sections.
*/
#test
{
    var cpt = 0

    // Standard syntax
    for var i = 0; i < 10; i += 1 do
        cpt += 1
    @assert(cpt == 10)

    // Alternative syntax with newlines
    for var i = 0; i < 10; i += 1
    {
        cpt += 1
    }
    @assert(cpt == 20)
}

/**
# Accessing Loop Index with '@index'

In all `for` variants, `@index` provides the current iteration index.
*/
#test
{
    var cpt = 0'u64

    for var i: u32 = 10; i < 15; i += 1 do
        cpt += @index
    @assert(cpt == 0 + 1 + 2 + 3 + 4)

    var cpt1 = 0'u64
    for var i = 10; i < 15; i += 1 do
        cpt1 += @index
    @assert(cpt1 == 0 + 1 + 2 + 3 + 4)
}

/**
# Using 'break' and 'continue' in 'for' Loops

`break` exits the loop early, and `continue` skips to the next iteration.
*/
#test
{
    var sum = 0

    for var i = 0; i < 10; i += 1
    {
        if i == 5 do
            break
        sum += i
    }
    @assert(sum == 0 + 1 + 2 + 3 + 4)

    sum = 0
    for var i = 0; i < 10; i += 1
    {
        if i % 2 == 0 do
            continue
        sum += i
    }
    @assert(sum == 1 + 3 + 5 + 7 + 9)
}

/**
# Nested 'for' Loops

Swag supports nested loops. In nested contexts, `@index` refers to the current innermost loop.
*/
#test
{
    var result = 0'u64

    for var i = 0; i < 5; i += 1
    {
        for var j = 0; j < 5; j += 1
        {
            result += @index
        }
    }

    @assert(result == 10 * 5)
}

/**
# Iterating Over Arrays with 'for'

You can also use `for` to iterate over arrays and collections.
(Although `foreach` is generally preferred.)
*/
#test
{
    var array = [1, 2, 3, 4, 5]
    var sum   = 0

    for var i = 0; i < @countof(array); i += 1
    {
        sum += array[i]
    }

    @assert(sum == 1 + 2 + 3 + 4 + 5)
}
