#test
{
    var a: string
    if a
        @assert(false)
    a = ""
    if !a
        @assert(false)
}