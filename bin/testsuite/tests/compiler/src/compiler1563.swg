var G = 0

//#[Swag.PrintBc]
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