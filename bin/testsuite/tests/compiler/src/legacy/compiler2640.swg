#test
{
    let f = 666
    var g = {factor: f}
    @assert(g.factor == 666)

    let v = cast(*void) &g

    let p = cast(*#decltype(g)) v
    @assert(p.factor == 666)
}
