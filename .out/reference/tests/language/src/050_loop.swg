#test
{
    // loop are used to iterate a given amount of time
    // The loop expression value is evaluated once.
    {
        cpt := 0
        loop 10 cpt += 1
        @assert(cpt == 10)
    }

    // The intrinsic @index returns the current index of the loop.
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

        loop i: 5
        {
            cpt += i
            cpt1 += @index
        }

        @assert(cpt == 0+1+2+3+4)
        @assert(cpt1 == cpt)
    }

    // loop can be used on every types that accept the @countof intrinsic
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
}