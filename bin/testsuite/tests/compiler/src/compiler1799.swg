#global testerror "Err0511"

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