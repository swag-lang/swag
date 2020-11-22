#test
{
    x := 0
    switch
    {
        case x == 0: x = 50
        case x == 1: @assert(false)
    }

    @assert(x == 50)

    switch
    {
        case x == 50:
            x = 5
            fallthrough
        case x == 10:
            x += 1
    }

    @assert(x == 6)
}