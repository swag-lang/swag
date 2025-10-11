var G  = 0
var G1 = 0

struct ErrTata {}

#[Swag.Discardable]
func tata()->s32 throw
{
    G += 1
    if G == 3 do
        throw ErrTata{}
    return 0
}

//#[Swag.PrintGenBc]
func toto() throw
{
    try
    {
        defer #err G1 += 1
        tata()
        defer #err G1 += 1
        tata()
        defer #err G1 += 1
        tata()
    }
}

#test
{
    catch toto()
    @assert(G == 3)
    @assert(G1 == 3)
}
