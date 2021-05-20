#[Swag.tls]
var G: string = "titi"

//#[Swag.printbc]
#test
{
    @assert(G == "titi")
}