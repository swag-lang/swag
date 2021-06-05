var G = 0
func toto() throw
{
    errdefer G = 1
    throw "toto"
}

#test
{
    catch toto()
    @assert(G == 1)
}