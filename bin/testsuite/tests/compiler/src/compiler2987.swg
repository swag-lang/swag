#test
{
    var x   = [1, 2]
    var ptr = &x
    ptr = 1 + ptr
    @assert(dref ptr == 2)
}
