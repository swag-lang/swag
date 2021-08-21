#global testerror

namespace A.B
{
    func toto()
    {
    }
}

#test
{
    toto := &A.B.toto
    alias toto = A.B.toto
}