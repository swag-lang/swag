enum RGB { R, G, B }
func toto() => RGB.R
#test
{
    switch toto()
    {
    case G:
        @assert(false)
    }
}
