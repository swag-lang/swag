#global testerror "Err0626"

namespace A
{
    namespace B
    {
        func toto()
        {
        }
    }
}

#test
{
    var toto = &A.B.toto
    typealias toto = A.B.toto
}
