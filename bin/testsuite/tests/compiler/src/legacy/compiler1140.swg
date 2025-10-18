//#global testerror "Err0090"

namespace A.B
{
    using A.C
}

namespace A.B
{
    func toto1() => toto()
}

namespace A.C
{
    func toto() {}
}
