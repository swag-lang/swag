#test
{
    var x = [1, 2]
    let s = @mkslice(&x[0], 2)
    @assert(s[0] == 1)
    @assert(s[1] == 2)
    @assert(@countof(s) == 2)
}