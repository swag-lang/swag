#global testerror "Err0631"

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