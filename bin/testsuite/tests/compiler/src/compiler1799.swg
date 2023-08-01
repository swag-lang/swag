#global testerror

namespace A.B
{
    func toto()
    {
    }
}

#test
{
    var toto = &A.B.toto
    typealias toto = A.B.toto
}