#test
{
    var a   = 0
    var b   = 0
    let pa1 = &a
    let pa2 = &a
    let pb  = &b
    @assert(pa1 == pa2)
    @assert(pb > pa1)
    @assert(pa1 < pb)
    @assert(pb >= pa1)
    @assert(pa1 <= pb)
    @assert(pa1 != pb)
}
