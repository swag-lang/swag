#global testerror "Err0394"

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