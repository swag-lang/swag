/*func T()-> {a,b: s32}
{
}*/

//#[Swag.PrintBc]
#test
{
    var x: { a, b: s32 }
    x = {1, 2}
    @assert(x.item0 == 1)
    @assert(x.item1 == 2)
}
