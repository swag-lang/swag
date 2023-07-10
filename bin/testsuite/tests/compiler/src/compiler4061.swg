//#global skip
#test
{
    let toto = false
    let r = ref &toto
    if r and r
    {
        @assert(false)
    }
}