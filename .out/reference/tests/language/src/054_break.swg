#test
{
    // 'break' is used to exit a loop, while, for, switch
    {
        loop 10
            break
        for i:= 0; i < 10; i += 1
            break
        while true
            break
    }

    // By default, 'break' will exit the parent scope only
    {
        cpt := 0
        loop 10
        {
            loop 10
            {
                break   // Exit the inner loop
            }

            cpt += 1
        }

        @assert(cpt == 10)
    }

    // You can name a scope with a 'label', and exit to the end of it
    {
        cpt := 0
        label BigScope
        {
            loop 10
            {
                cpt += 1
                break BigScope
            }

            @assert(false)
        }

        @assert(cpt == 1)
    }

    // When used with a label, a continue is a way to go back
    // to the start of the label
    {
        cpt := 0
        label down
        {
            cpt += 1
            if cpt == 5
                break
            continue
        }

        @assert(cpt == 5)
    }
}