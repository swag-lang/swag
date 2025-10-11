struct ErrToto {}

var G = 0
func toto() throw
{
    defer #err G = 1
    throw ErrToto{}
}

#test
{
    catch toto()
    @assert(G == 1)
}
