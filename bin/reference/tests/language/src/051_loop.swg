/**
`loop` is used to iterate a given number of times. It's a versatile construct in Swag, allowing for various iteration patterns.
*/

/**
The loop expression is evaluated **once** and must be a **positive value**.
*/
#test
{
    var cpt = 0
    loop 10:                        // Loops 10 times
        cpt += 1
    @assert(cpt == 10)
}

/**
The compiler keyword `#index` returns the current index of the loop, starting at 0.
*/
#test
{
    var cpt = 0'u64
    loop 5
    {
        cpt += #index
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
}

/**
You can name the loop index if you want.
*/
#test
{
    var cpt  = 0
    var cpt1 = 0

    loop i in 5                     // index is named 'i'
    {
        cpt += i
        cpt1 += #index              // #index is still available, even when named
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
    @assert(cpt1 == cpt)
}

/**
`loop` can be used on any type that supports the `@countof` intrinsic, such as slices, arrays, and strings. We'll explore using it with structs later.
*/
#test
{
    var arr = [10, 20, 30, 40]
    #assert @countof(arr) == 4

    var cpt = 0
    loop arr:                       // The array contains 4 elements, so the loop count is 4
        cpt += arr[#index]
    @assert(cpt == 10 + 20 + 30 + 40)
}

/**
> WARNING:
> On a string, it will loop over each byte, **not** runes (if a rune is encoded in more than one byte). To iterate over runes, use the Std.Core module.
*/
#test
{
    var cpt = 0
    loop "⻘":
        cpt += 1

    // cpt is equal to 3 because '⻘' is encoded with 3 bytes
    @assert(cpt == 3)
}

/**
You can loop in reverse order by adding the `back` modifier after the `loop`.
*/
#test
{
    var cpt = 0

    // Index will be 2, 1, then 0.
    loop #back 3
    {
        if cpt == 0:
            @assert(#index == 2)
        elif cpt == 1:
            @assert(#index == 1)
        elif cpt == 2:
            @assert(#index == 0)

        cpt += 1
    }
}

/**
# break, continue
`break` and `continue` can be used inside a loop.
*/

/**
Exit a loop early with `break`.
*/
#test
{
    var cpt = 0
    loop x in 10
    {
        if x == 5:
            break
        cpt += 1
    }

    @assert(cpt == 5)
}

/**
Use `continue` to skip the remainder of the loop body and return to the loop evaluation.
*/
#test
{
    var cpt = 0
    loop x in 10
    {
        if x == 5:
            continue              // Skip iteration when x is 5
        cpt += 1
    }

    @assert(cpt == 9)
}

/**
# Ranges
`loop` can also iterate over a **range** of signed values.
*/

/**
Use 'to' to loop from a given value *to* another value. The first value must be lower than or equal to the upper bound.
*/
#test
{
    var count = 0
    var sum   = 0
    loop i in -1 to 1              // loop from -1 to 1, inclusive
    {
        count += 1
        sum += i
    }

    @assert(sum == 0)
    @assert(count == 3)
}

/**
Exclude the last value with 'until' instead of 'to'.
*/
#test
{
    // Loop from 1 to 2, excluding 3
    var cpt = 0
    loop i in 1 until 3
    {
        cpt += i
    }

    @assert(cpt == 1 + 2)
}

/**
With a range, you can also loop in reverse order by adding the `back` modifier.
*/
#test
{
    // Loop from 5 to 0
    loop #back 0 to 5
    {
    }

    // Loop from 1 to -1
    loop #back -1 to 1
    {
    }

    // Loop from 1 to -2, excluding the upper limit.
    loop #back -2 until 2
    {
    }
}

/**
# Infinite loop
A loop without an expression but with a block is infinite, equivalent to `while true {}`.
*/
#test
{
    loop
    {
        if #index == 4:            // #index is still valid, but cannot be renamed
            break
    }
}

/**
# where
The `where` clause allows you to filter loop iterations based on a condition. When using `where`, the loop will only execute for indexes or values that satisfy the condition specified in the `where` clause. This provides a convenient way to add additional logic to your loops, ensuring that only certain iterations are executed.
*/

/**
The `where` clause can be added directly after the `loop` statement. This clause applies a condition to the loop's index or value, and only those that meet the condition will be processed.
*/
#test
{
    var result = 0

    // The loop will only visit 'even' indexes from 0 to 9.
    loop i in 10 where i % 2 == 0
    {
        result += i
    }

    @assert(result == 0 + 2 + 4 + 6 + 8)  // Result is the sum of even numbers within the range
}

/**
When looping over arrays, the `where` clause can filter elements based on their value or index.
*/
#test
{
    var arr = [10, 21, 30, 41, 50]
    var sumOfEvens = 0

    // This loop sums up only the even numbers in the array.
    loop i in arr where arr[i] % 2 == 0
    {
        sumOfEvens += arr[i]
    }

    @assert(sumOfEvens == 10 + 30 + 50)  // Only the even numbers (10, 30, 50) are summed
}

/**
You can use the `where` clause to create complex filtering conditions. For example, you can combine multiple logical expressions to refine which elements the loop should process.
*/
#test
{
    var arr = [10, 15, 20, 25, 30, 35]
    var filteredSum = 0

    // Sum only even numbers greater than 15.
    loop i in arr where arr[i] % 2 == 0 and arr[i] > 15
    {
        filteredSum += arr[i]
    }

    @assert(filteredSum == 20 + 30)  // Only 20 and 30 meet the condition
}

/**
The `where` clause can also be applied to loops over ranges, allowing for precise control over which range values are included in the loop.
*/
#test
{
    var sumOfPositiveEvens = 0

    // Loop over the range from -5 to 5, but only include positive even numbers.
    loop i in -5 to 5 where i > 0 and i % 2 == 0
    {
        sumOfPositiveEvens += i
    }

    @assert(sumOfPositiveEvens == 2 + 4)  // Only positive even numbers (2, 4) are summed
}

/**
You can combine the `back` modifier with the `where` clause to filter values while iterating in reverse.
*/
#test
{
    var arr = [10, 20, 30, 40, 50]
    var reversedSum = 0

    // Loop through the array in reverse, summing only the even values.
    loop #back i in arr where arr[i] % 2 == 0
    {
        reversedSum += arr[i]
    }

    @assert(reversedSum == 50 + 40 + 30 + 20 + 10)  // Sums all even values in reverse order
}

/**
The `where` clause supports complex logical expressions, enabling intricate filtering criteria directly within the loop.
*/
#test
{
    var arr = [10, 25, 30, 45, 50, 65]
    var complexSum = 0

    // Sum elements that are either even or greater than 40.
    loop i in arr where arr[i] % 2 == 0 or arr[i] > 40
    {
        complexSum += arr[i]
    }

    @assert(complexSum == 10 + 30 + 45 + 50 + 65)  // Values matching the complex condition are summed
}

/**
The `where` clause is a powerful feature in Swag that enhances the flexibility of loops by allowing you to add conditional logic directly within the loop construct. This allows for cleaner, more readable code by avoiding the need for additional `if` statements inside the loop body.
*/
