interface TOTO
{
    func print(me, x: string);
}

struct TITI {}

impl TOTO for TITI
{
    func impl print(me1: *TITI, x: string) {}
}

//#[Swag.PrintBc]
#test
{
    /*    var x: TOTO
    var y: TITI
    x = y*/
}
