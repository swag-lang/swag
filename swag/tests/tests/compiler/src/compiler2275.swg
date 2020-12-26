#test
{
    var ptr: **void
    ptr += 1
    @assert(cast(uint) ptr == @sizeof(*void))
}