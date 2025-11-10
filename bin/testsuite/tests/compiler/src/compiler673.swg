//#[Swag.PrintBc]
#test
{
    var a: { a, b: s32 }
    a.item0 = 1
    a.item1 = 2
    @assert(a.item0 == 1)
    @assert(a.item1 == 2)
}
