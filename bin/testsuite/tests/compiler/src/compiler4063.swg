//#global skip
#test
{
    toto := false
    r := ref &toto
    while r
    {
        @assert(false)
    }
}