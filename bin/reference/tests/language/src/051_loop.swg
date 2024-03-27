/**
`loop` are used to iterate a given amount of time.
*/

/**
The loop expression value is evaluated **once**, and must be a **positive value**.
*/
#test
{
    var cpt = 0
    loop 10 do
        cpt += 1
    @assert(cpt == 10)
}

/**
The intrinsic `#index` returns the current index of the loop, starting at 0.
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
But you can name that index if you want.
*/
#test
{
    var cpt  = 0
    var cpt1 = 0

    loop i: 5 // index is named 'i'
    {
        cpt += i
        cpt1 += #index // #index is always available, even when named
    }

    @assert(cpt == 0 + 1 + 2 + 3 + 4)
    @assert(cpt1 == cpt)
}

/**
`loop` can be used on every types that accept the `@countof` intrinsic. So you can loop on a slice, an array, a string... and we'll see later, even on a struct.
*/
#test
{
    var arr = [10, 20, 30, 40]
    #assert @countof(arr) == 4

    var cpt = 0
    loop arr do
        cpt += arr[#index]
    @assert(cpt == 10 + 20 + 30 + 40)
}

/**
> WARNING:
> On a string, it will loop for each byte, **not** runes (if a rune is encoded in more than one byte). If you want to iterate on all runes, you will have to use the Std.Core module.
*/
#test
{
    var cpt = 0
    loop "⻘" do
        cpt += 1

    // cpt is equal to 3 because '⻘' is encoded with 3 bytes
    @assert(cpt == 3)
}

/**
You can loop in reverse order by adding the modifier `back` just after the `loop`.
*/
#test
{
    var cpt = 0

    // Index will be 2, 1 and then 0.
    loop,back 3
    {
        if cpt == 0 do
            @assert(#index == 2)
        elif cpt == 1 do
            @assert(#index == 1)
        elif cpt == 2 do
            @assert(#index == 0)

        cpt += 1
    }
}

/**
# break, continue
`break` and `continue` can be used inside a loop.
*/

/**
You can exit a loop with `break`.
*/
#test
{
    var cpt = 0
    loop x: 10
    {
        if x == 5 do
            break
        cpt += 1
    }

    @assert(cpt == 5)
}

/**
You can force to return to the loop evaluation with `continue`.
*/
#test
{
    var cpt = 0
    loop x: 10
    {
        if x == 5 do
            continue
        cpt += 1
    }

    @assert(cpt == 9)
}

/**
# Ranges
Loop can also be used to iterate on a **range** of signed values.
*/

/**
You can add 'to' to loop from a given value *to* a given value. Note that the range should always have a first value lower or equal to the upper bound.
*/
#test
{
    var count = 0
    var sum   = 0
    loop i: -1 to 1 // loop from -1 to 1, all included
    {
        count += 1
        sum += i
    }

    @assert(sum == 0)
    @assert(count == 3)
}

/**
You can exclude the last value with by using 'until' instead of 'to'.
*/
#test
{
    // Will loop from 1 to 2 and **not** 1 to 3
    var cpt = 0
    loop i: 1 until 3
    {
        cpt += i
    }

    @assert(cpt == 1 + 2)
}

/**
With a range, you can also loop in reverse order if you add the `back` modifier.
*/
#test
{
    // Loop from 5 to 0
    loop,back 0 to 5
    {
    }

    // Loop from 1 to -1
    loop,back -1 to 1
    {
    }

    // Loop from 1 to -2 because we exclude the upper limit.
    loop,back -2 until 2
    {
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
        if #index == 4 do
            break
    }
}
