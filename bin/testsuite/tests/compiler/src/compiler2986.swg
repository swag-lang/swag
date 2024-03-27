#test
{
    var x   = [1, 2]
    let ptr = &x
    @assert(ptr[0] == 1)
    @assert(dref ptr == 1)
}
