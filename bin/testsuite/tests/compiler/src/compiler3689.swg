#global testerror "assertion failed"

enum RGB { R; G; B; }
func toto()->RGB
{
    return .G
}

#test
{
    @assert(toto() == .R)
}