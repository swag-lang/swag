enum RGB { R, G, B }
func toto()->RGB
{
    return .R
}

#test
{
    @assert(toto() == .R)
}
