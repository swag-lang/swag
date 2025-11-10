#test
{
    var t  = [1, 2]
    let x: *s32 = &t
    let y  = dref &t[1]
    @assert(y == 2)
}
