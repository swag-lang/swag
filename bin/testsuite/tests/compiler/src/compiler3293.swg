#test
{
    var a = 0
    if a == 1
        @assert(false)
    elif a == 0
        a = 2
    @assert(a == 2)
}