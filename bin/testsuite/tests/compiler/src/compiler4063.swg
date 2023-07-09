//#global skip
#test
{
    var toto = false
    var r = ref &toto
    while r
    {
        @assert(false)
    }
}