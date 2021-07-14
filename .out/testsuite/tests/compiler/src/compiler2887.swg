#[Swag.tls]
var G: s32 = 5

//#[Swag.PrintBc]
#test
{
    @assert(G == 5)
    G = 0
    @assert(G == 0)
}