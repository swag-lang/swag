enum RGB
{
    R
    G
    B
}

func toto()->RGB => RGB.R
#test
{
    @assert(toto() == RGB.R)
}