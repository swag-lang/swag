#global testerror "Err0009"

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