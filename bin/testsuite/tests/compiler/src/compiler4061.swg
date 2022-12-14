//#global skip
#test
{
    toto := false
    r := ref &toto
    if r and r
    {
        @assert(false)
    }
}