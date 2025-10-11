//#global skip
#test
{
    let a = {0, 1}
    var b = {0, 1}
    @assert(a == b)
    b = {0, 2}
    @assert(a != b)
}
