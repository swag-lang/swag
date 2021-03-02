#global if true
#global private
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

#[swag.macro]
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