var G = 0
func toto()
{
    defer #err G = 1
}

#test
{
    toto()
    @assert(G == 0)
}
