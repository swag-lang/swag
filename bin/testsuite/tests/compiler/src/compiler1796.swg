var G = 0

namespace A.B
{
    func toto(x: s32)
    {
        G += x
    }
}

#test
{
    alias NP = A.B
    NP.toto(10)
    @assert(G == 10)
}