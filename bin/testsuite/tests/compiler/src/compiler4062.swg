//#global skip
#test
{
    var toto = false
    var r = ref &toto
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