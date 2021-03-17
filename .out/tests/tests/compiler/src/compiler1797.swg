
#global testerror

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
    using NP = A.B
    toto(10)
    @assert(G == 10)
}