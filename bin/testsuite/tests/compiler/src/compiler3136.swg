#test
{
    var x: *u32
    y := bitcast(u64) x
    @assert(y == 0)
}