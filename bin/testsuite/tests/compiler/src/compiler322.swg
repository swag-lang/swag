
//#[Swag.PrintBc]
#test
{
    var a: s32 = 666
    let b  = &a
    var c: *s32
    c = b
    @assert(dref b == 666)
    @assert(dref c == 666)
}
