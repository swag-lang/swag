/**
`loop` are used to iterate a given amount of time.
*/

#test
{
    // The loop expression value is evaluated **once**, and must be a positive value.
    {
        var cpt = 0
        loop 10 // Loops 10 times
            cpt += 1
        @assert(cpt == 10)
    }

    // The intrinsic `@index` returns the current index of the loop (starting at 0).
    {
        var cpt = 0'u64
        loop 5
        {
            cpt += @index
        }

        @assert(cpt == 0+1+2+3+4)
    }

    // You can name that index if you want.
    {
        var cpt = 0
        var cpt1 = 0

        loop i: 5   // index is named 'i'
        {
            cpt  += i
            cpt1 += @index  // @index is always available, even when named
        }

        @assert(cpt  == 0+1+2+3+4)
        @assert(cpt1 == cpt)
    }
}

/**
`loop` can be used on every types that accept the `@countof` intrinsic. So you can loop on a slice, an array, a string... and we'll see later, even on a struct.
*/
#test
{
    var arr = [10, 20, 30, 40]
    #assert @countof(arr) == 4

    var cpt = 0
    loop arr    // The array contains 4 elements, so the loop count is 4
        cpt += arr[@index]
    @assert(cpt == 10+20+30+40)
}

/**
**Warning !**
On a string, it will loop for each byte, **not** runes (if a rune is encoded in more than one byte). If you want to iterate on all runes, you will have to use the `Std.Core` module.
*/
#test
{
    var cpt = 0
    loop "⻘"
        cpt += 1

    // cpt is equal to 3 because '⻘' is endoded with 3 bytes
    @assert(cpt == 3)
}

/**
# break, continue
`break` and `continue` can be used inside a loop.
*/
#test
{
    // You can exit a loop with `break`.
    {
        var cpt = 0
        loop x: 10
        {
            if x == 5
                break
            cpt += 1
        }

        @assert(cpt == 5)
    }

    // You can force to return to the loop evaluation with `continue`.
    {
        var cpt = 0
        loop x: 10
        {
            if x == 5
                continue // Do not count 5
            cpt += 1
        }

        @assert(cpt == 9)
    }
}

/**
# Ranges
Loop can also be used to iterate on a range of signed values.
*/
#test
{
    // Syntax is `lower bound..upper bound`
    {
        var count = 0
        var sum = 0
        loop i: -1..1   // loop from -1 to 1, all included
        {
            count += 1
            sum += i
        }

        @assert(sum == 0)
        @assert(count == 3)
    }

    // With a range, you can loop in reverse order.
    {
        // Loop from 5 to 0
        loop 5..0
        {
        }

        // Loop from 1 to -1
        loop 1..-1
        {
        }
    }

    // You can exclude the last value with the `..<` syntax.
    {
        // Will loop from 1 to 2 and **not** 1 to 3
        var cpt = 0
        loop i: 1..<3
        {
            cpt += i
        }

        @assert(cpt == 1+2)
    }
}

/**
# Infinite loop
A loop without an expression but with a block is infinite. This is equivalent to `while true {}`.
*/
#test
{
    loop
    {
        if @index == 4 // @index is still valid in that case (but cannot be renamed)
            break
    }
}
