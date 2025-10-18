#global testerror "Err0503"

interface TOTO
{
    func print(me, str: string);
}

struct TITI {}

impl TOTO for TITI
{
    func impl print(y: bool, x: string) {}
}
