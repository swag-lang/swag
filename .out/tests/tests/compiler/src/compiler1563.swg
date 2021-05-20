var G = 0

//#[Swag.printbc]
#test
{
    defer
    {
        loop 10
            G += 1
        @assert(G == 10)
    }

    G = 0
}