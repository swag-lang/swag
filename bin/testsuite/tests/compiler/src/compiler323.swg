//#[Swag.PrintBc]
#test
{
    var a: s32 = 1
    var b  = &a
    dref b = 666; @assert(a == 666)
    @assert(dref b == a)
}
