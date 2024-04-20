#global testerror "Err0017"

namespace A
{
    func toto() {}
}

private
{
    func toto() {}
}

#test
{
    using A//, B
    var a = &toto
}