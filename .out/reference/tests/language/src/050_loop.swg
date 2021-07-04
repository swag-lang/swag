#test
{
    // loop are used to iterate a given amount of time
    // The loop expression value is evaluated once.
    // The loop expression must be a positive value.
    {
        cpt := 0
        loop 10 cpt += 1
        @assert(cpt == 10)
    }

    // The intrinsic '@index' returns the current index of the loop.
    {
        cpt := 0'uint
        loop 5
        {
            cpt += @index
        }

        @assert(cpt == 0+1+2+3+4)
    }

    // You can name that index if you want
    {
        cpt := 0
        cpt1 := 0

        loop i: 5   // index is named 'i'
        {
            cpt += i
            cpt1 += @index  // @index is always available, even when named
        }

        @assert(cpt == 0+1+2+3+4)
        @assert(cpt1 == cpt)
    }

    // loop can be used on every types that accept the '@countof' intrinsic
    {
        arr := @[1, 2, 3, 4]
        @assert(@countof(arr) == 4)

        cpt := 0
        loop arr cpt += arr[@index]
        @assert(cpt == 1+2+3+4)
    }

    // On a string, it will loop for each byte, not character (if a character is more than one byte)
    {
        cpt := 0
        loop "⻘"
        {
            cpt += 1
        }

        @assert(cpt == 3)
    }

    // You can exit a loop with 'break'
    {
        cpt := 0
        loop x: 10
        {
            if x == 5
                break
            cpt += 1
        }

        @assert(cpt == 5)
    }

    // You can force to return to the loop evaluation with 'continue'
    {
        cpt := 0
        loop x: 10
        {
            if x == 5
                continue // Do not count 5
            cpt += 1
        }

        @assert(cpt == 9)
    }

    // Loop can also be used to iterate on a range of signed values
    // Syntax is <lower bound>..<upper bound>
    {
        count := 0
        sum := 0
        loop i: -1..1   // loop from -1 to 1, all included
        {
            count += 1
            sum += i
        }

        @assert(sum == 0)
        @assert(count == 3)
    }

    // With a range, you can loop in reverse order
    // Loop from 5 to 0
    loop 5..0
    {
    }

    // Loop from 1 to -1
    loop 1..-1
    {
    }

    // A loop without an expression but with a block is infinite
    // This is equivalent to while true {}
    // @index is still valid in that case (but cannot be renamed)
    loop
    {
        if @index == 4
            break
    }
}