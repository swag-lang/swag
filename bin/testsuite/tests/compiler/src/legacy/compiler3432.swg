struct X
{
    v:   func||(s32)
}

var G = 0

#test
{
    var x: X
    x.v = func(x: s32) { G = x; }
    x.v(5)
    @assert(G == 5)
}
