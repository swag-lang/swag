#test
{
    x := @[1, 2]
    ptr := &x
    ptr = 1 + ptr
    @assert(dref ptr == 2)
}