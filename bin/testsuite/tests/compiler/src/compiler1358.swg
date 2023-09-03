#global testerror "Syn0133"

interface TOTO
{
	print: func(self, bool)
}

struct TITI
{
    val: s64 = 666
}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func print(using self, b: bool)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var XX: TOTO
    TOTO.print(XX, true)
}