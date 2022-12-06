var G: [] u8 = [1, 2, 3]
var G1 = 0

func toto()->[..] u8
{
    G1 += 1
    return G
}

//#[Swag.PrintBc]
#test
{
    visit i: toto()
    {
    }

    @assert(G1 == 1)
}