#test
{
    x := @[1, 2, 3]
    cpt := 0
    addr := &x[0]
    offset := 1
    addr, cpt += 1
    @assert(cpt == 1)
    @assert(:addr == 2)
}
