//#global testerror

const G = 1
var G1 = 21

#[Swag.Overload]
func toto() #validif G == 0
{
    G1 = 0
}

#[Swag.Overload]
func toto() #validif G == 1
{
    G1 = 1
}


#test
{
    toto()
    toto()
    @assert(G1 == 1)
}