//#global testerror "Err0712"

namespace A
{
    namespace B
    {
        using A.C
    }
}

namespace A
{
    namespace B
    {
        func toto1() => toto()
    }
}

namespace A
{
    namespace C
    {
        func toto()
        {
        }
    }
}
