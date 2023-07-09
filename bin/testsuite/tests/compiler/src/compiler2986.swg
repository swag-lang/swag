#test
{
    var x = [1, 2]
    var ptr = &x
    @assert(ptr[0] == 1)
    @assert(dref ptr == 1)
}