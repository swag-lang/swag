namespace A
{
    func titi() => 0
    func toto() => titi()
}

namespace B
{
    func titi() => 0
    func toto() => titi()

    func tata()
    {
        let a = A.toto()
    }
}
