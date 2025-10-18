#global testerror "Err0306"

interface TOTO
{
    func print(me, x: s32);
}

struct TITI {}

impl TOTO for TITI
{
    func impl print() {}
}
