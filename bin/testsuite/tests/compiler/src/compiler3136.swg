#test
{
    var x: *u32
    let y  = cast #bit (u64) x
    @assert(y == 0)
}
