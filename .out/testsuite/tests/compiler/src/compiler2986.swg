#test
{
    x := @[1, 2]
    ptr := &x
    @assert(ptr[0] == 1)
    @assert(dref ptr == 1)
}