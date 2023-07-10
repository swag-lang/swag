//#global skip
#test
{
    let toto = false
    let r = ref &toto
    while r
    {
        @assert(false)
    }
}