//#global skip
var G = 0
#test
{
    var v: closure(s32)
    v = closure||(x: s32)
    {
        G = x
    }
    v(5)
    @assert(G == 5)
}
