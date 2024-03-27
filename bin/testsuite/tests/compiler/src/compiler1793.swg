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
    namealias toto1 = A
    toto1.toto()
    @assert(G == 1)
}
