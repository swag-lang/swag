//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var a = [1, 2, 3, 4]
    var b = {a}
    @assert(b.item0[0] == 1)
    @assert(b.item0[1] == 2)
    @assert(b.item0[2] == 3)
    @assert(b.item0[3] == 4)
    @assert(b.a[0] == 1)
    @assert(b.a[1] == 2)
    @assert(b.a[2] == 3)
    @assert(b.a[3] == 4)
}
