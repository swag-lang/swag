#global testerror "Err0205"

interface TOTO
{
	func print(self, b: bool);
}

struct TITI
{
    val: s64 = 666
}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func impl print(using self, b: bool)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var XX: TOTO
    TOTO.print(XX, true)
}