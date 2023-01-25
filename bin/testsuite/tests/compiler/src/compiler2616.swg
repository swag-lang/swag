//#global testerror

const G = 1
var G1 = 21

func toto() #selectifonce G == 0
{
    G1 = 0
}

func toto() #selectifonce G == 1
{
    G1 = 1
}


#test
{
    toto()
    toto()
    @assert(G1 == 1)
}