//#global skip
#test
{
    var toto = false
    let r    = #ref &toto
    while r
    {
        @assert(false)
    }
}
