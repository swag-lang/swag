#test
{
    var x: ()->void
    if x || x
        @assert(false)
}