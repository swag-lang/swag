#global testerror "Err0004"

namespace A.B
{
    func toto()
    {
    }
}

#test
{
    var toto = &A.B.toto
    alias toto = A.B.toto
}