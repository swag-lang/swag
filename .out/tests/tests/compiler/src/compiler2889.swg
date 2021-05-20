#[Swag.tls]
var G: [2] string = @["titi", "toto"]

//#[Swag.printbc]
#test
{
    @assert(G[0] == "titi")
    @assert(G[1] == "toto")
}