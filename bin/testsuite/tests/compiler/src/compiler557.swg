/*func T()-> {a,b: s32}
{
}*/

//#[Swag.PrintBc]
#test
{
    var x, y: { a, b: s32 }
    x = {1, 2}
    y = x
    @assert(y.item0 == 1)
    @assert(y.item1 == 2)
}
