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
    foreach i in toto()
    {
    }

    @assert(G1 == 1)
}
