#global #if true
#global namespace NP
using BB, AA

namespace BB
{
}

namespace AA
{
    func toto() {}
}

func titi()
{
    func tata()
    {
        toto()
    }

    tata()
}
