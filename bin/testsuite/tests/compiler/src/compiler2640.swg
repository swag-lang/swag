#test
{
    var f = 666
    var g = {factor = f}
    @assert(g.factor == 666)

    var v = cast(*void) &g

    var p = cast(*@decltype(g)) v
    @assert(p.factor == 666)
}