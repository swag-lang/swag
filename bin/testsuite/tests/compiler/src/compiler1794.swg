var G = 0

namespace A
{
    func toto()
    {
        G += 1
    }
}

#test
{
    namealias toto1 = A.toto
    toto1()
    @assert(G == 1)
}