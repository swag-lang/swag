enum RGB { R; G; B; }
func toto() => RGB.R
#test
{
    @assert(toto() == .R)
}