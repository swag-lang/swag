#global testerror

enum RGB { R; G; B; }
func toto()->RGB
{
    return .G
}

#test
{
    @assert(toto() == .R)
}