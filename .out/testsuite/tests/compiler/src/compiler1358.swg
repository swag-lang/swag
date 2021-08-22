#global testerror

interface TOTO
{
	print: func(self)
}

struct TITI
{
    val: s64 = 666
}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func print(using self)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var XX: TOTO
    TOTO.print(XX, true)
}