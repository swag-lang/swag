//#global skip
#test
{
    toto := false
    r := ref &toto
    if r
    {
        @assert(false)
    }

    r = true
    if r
        @assert(true)
    else
        @assert(false)

    if !r
        @assert(false)
    else
        @assert(true)
}