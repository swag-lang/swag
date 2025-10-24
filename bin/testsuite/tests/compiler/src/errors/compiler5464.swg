//#global testerror "assertion failed"

enum RGB { R, G, B }

#[Swag.Inline]
func toto()->RGB
{
    return .G
}

#test
{
    @assert(toto() == .G)
}
