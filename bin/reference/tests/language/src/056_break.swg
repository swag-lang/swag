#test
{
    // We have already seen than `break` is used to exit a `loop`, `visit`, `while`, `for`, `switch`.
    {
        loop 10
            break
        for i := 0; i < 10; i += 1
            break
        while false
            break
    }

    // By default, `break` will exit the parent scope only.
    {
        cpt := 0
        loop 10
        {
            loop 10
            {
                break   // Exit the inner loop...
            }

            // ...and continue execution here
            cpt += 1
        }

        @assert(cpt == 10)
    }

    // But you can **name a scope** with the `#scope` compiler keyword, and exit to the end of it with a `break`.
    {
        cpt := 0

        // Creates a scope named 'BigScope'
        #scope BigScope
        {
            loop 10
            {
                cpt += 1
                break BigScope  // Break to the outer scope...
            }

            @assert(false)
        }

        // ...and continue execution here
        @assert(cpt == 1)
    }

    // When used with a scope, a continue is a way to go back to the start of the scope.
    {
        cpt := 0
        #scope Loop
        {
            cpt += 1
            if cpt == 5
                break
            continue    // Loop
        }

        @assert(cpt == 5)
    }

    // You are not obliged to name the scope, so this can also be used (for example) as an alternative of a bunch of if/else.
    {
        cpt := 0
        #scope
        {
            if cpt == 1
            {
                @assert(cpt == 1)
                break
            }

            if cpt == 2
            {
                @assert(cpt == 2)
                break
            }

            if cpt == 3
            {
                @assert(cpt == 3)
                break
            }
        }
    }

    // Note that a scope can be followed by a simple statement, not always a block.
    {
        #scope Up loop 10
        {
            loop 10
            {
                if @index == 5
                    break Up
            }

            @assert(false)
        }
    }
}