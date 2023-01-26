#test
{
    var x: *u32
    y := cast,bit(u64) x
    @assert(y == 0)
}