//#global skip
var G = 0
#test
{
    var v: func||(s32)
    v = func||(x: s32) { G = x; }
    v(5)
    @assert(G == 5)
}
