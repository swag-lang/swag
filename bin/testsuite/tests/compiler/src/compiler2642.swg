#global #if true
#global namespace NP
using BB, AA

namespace BB
{
}

namespace AA
{
    func toto()
    {

    }
}

#[Swag.Macro]
func titi()
{
    func tata()
    {
        toto()
    }

    tata()
}

#test
{
    titi()
}