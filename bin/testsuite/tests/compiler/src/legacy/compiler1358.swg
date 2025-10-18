#global testerror "Err0388"

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
    func impl print(me, b: bool) {}
}

//#[Swag.PrintBc]
#test
{
    var XX: TOTO
    TOTO.print(XX, true)
}
