#test
{
    var x: s32 = 666
    y := dref &x
    @assert(x == 666)
}