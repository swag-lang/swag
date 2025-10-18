#global testerror "Err0230"

interface TOTO
{
    func print(me);
    func print1(me, x: s32);
}

struct TITI {}

impl TOTO for TITI
{
    func impl print(me) {}
}
