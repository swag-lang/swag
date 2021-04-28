#[swag.tls]
var G: s32 = 5

//#[swag.printbc]
#test
{
    @assert(G == 5)
    G = 0
    @assert(G == 0)
}