var G: [?] u8 = [1, 2, 3]
var G1 = 0

func toto()->[..] u8
{
    G1 += 1
    return G
}

//#[Swag.PrintBc]
#test
{
    foreach i in toto()
    {
    }

    @assert(G1 == 1)
}
