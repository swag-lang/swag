#module "compiler1799"
#unittest error

namespace A.B
{
    func toto()
    {
    }
}

#test
{
    toto := &A.B.toto
    using toto = A.B.toto
}