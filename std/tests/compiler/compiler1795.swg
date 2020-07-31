var G = 0

namespace A
{
    func toto(x: s32)
    {
        G += x
    }
}

#test
{
    using toto1 = A.toto
    toto1(10)
    @assert(G == 10)
}