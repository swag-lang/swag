#test
{
    var x      = [1, 2, 3]
    var cpt    = 0
    var addr   = &x[0]
    var offset = 1
    addr, cpt += 1
    @assert(cpt == 1)
    @assert(dref addr == 2)
}
