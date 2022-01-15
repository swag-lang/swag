//#global skip
var G1 = 0

func toto()->string
{
    G1 += 1
    return "G"
}

//#[Swag.PrintBc]
#test
{
    visit i: toto()
    {
    }

    @assert(G1 == 1)
}