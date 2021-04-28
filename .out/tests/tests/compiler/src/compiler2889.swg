#[swag.tls]
var G: [2] string = @["titi", "toto"]

//#[swag.printbc]
#test
{
    @assert(G[0] == "titi")
    @assert(G[1] == "toto")
}