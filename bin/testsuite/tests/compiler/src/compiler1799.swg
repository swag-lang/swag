#global testerror "Err0619"

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