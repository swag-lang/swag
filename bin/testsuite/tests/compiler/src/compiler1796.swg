var G = 0

namespace A
{
    namespace B
    {
        func toto(x: s32)
        {
            G += x
        }
    }
}

#test
{
    namealias NP = A.B
    NP.toto(10)
    @assert(G == 10)
}
