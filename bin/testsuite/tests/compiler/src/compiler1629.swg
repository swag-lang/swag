#run
{
    var arr: [64] u8
    ptr := cast(**void) &arr[0]
    ptr[0] = null
    @assert(ptr[0] == null)
}