#global testerror "Err0205"

interface TOTO
{
	func print(me, b: bool);
}

struct TITI
{
    val: s64 = 666
}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func impl print(using me, b: bool)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var XX: TOTO
    TOTO.print(XX, true)
}