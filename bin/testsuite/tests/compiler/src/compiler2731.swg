var G = 0
func toto()
{
    errdefer G = 1
}

#test
{
    toto()
    @assert(G == 0)
}