#[swag.tls]
var G: string = "titi"

#[swag.printbc]
#test
{
    @assert(G == "titi")
}