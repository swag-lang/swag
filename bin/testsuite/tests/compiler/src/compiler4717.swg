#global testerror "Err0024"
namespace A {
    struct(T) N {}
}

namespace B {
    using A
    struct N {}

    struct T
    {
        a: *N
    }
}
