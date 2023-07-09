#test
{
    var t = [1, 2]
    var x: *s32 = &t
    var y = dref &t[1]
    @assert(y == 2)
}