#global testerror "Err0004"
namespace A
{
    struct(T) N {}
}

namespace B
{
    using A
    struct N {}

    struct T
    {
        a:   *N
    }
}
