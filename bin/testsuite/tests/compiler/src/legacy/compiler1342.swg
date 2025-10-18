#global testerror "Err0503"

interface TOTO
{
    func print(me, str: string);
}

struct TITI {}

struct TATA {}

impl TOTO for TITI
{
    func impl print(me1: *TATA, x: string) {}
}

//#[Swag.PrintBc]
#test
{
    /*    var x: TOTO
    var y: TITI
    x = y*/
}
