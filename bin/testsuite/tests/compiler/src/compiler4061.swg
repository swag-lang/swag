//#global skip
#test
{
    var toto = false
    var r = ref &toto
    if r and r
    {
        @assert(false)
    }
}