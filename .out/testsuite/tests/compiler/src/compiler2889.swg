#[Swag.tls]
var G: [2] string = @["titi", "toto"]

//#[Swag.PrintBc]
#test
{
    @assert(G[0] == "titi")
    @assert(G[1] == "toto")
}