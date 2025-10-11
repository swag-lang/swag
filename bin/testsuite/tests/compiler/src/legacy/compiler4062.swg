//#global skip
#test
{
    var toto = false
    var r    = #ref &toto
    if r
    {
        @assert(false)
    }

    r = true
    if r do
        @assert(true)
    else do
        @assert(false)

    if !r do
        @assert(false)
    else do
        @assert(true)
}
