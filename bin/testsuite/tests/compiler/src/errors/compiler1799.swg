#global testerror "Err0232"

namespace A.B
{
    func toto() {}
}

#test
{
    var toto = &A.B.toto
    alias toto = A.B.toto
}
