var G = 0
var G1 = 0

#[Swag.discardable]
func tata()->s32 throw
{
    G += 1
    if G == 3 throw "tata"
    return 0
}

//#[Swag.PrintBc]
func toto() throw
{
    try
    {
        errdefer G1 += 1
        tata()
        errdefer G1 += 1
        tata()
        errdefer G1 += 1
        tata()
    }
}

#test
{
    catch toto()
    @assert(G == 3)
    @assert(G1 == 3)
}