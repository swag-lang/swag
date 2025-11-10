#global testerror "Err0283"

interface TOTO
{
    func print(me);
}

struct TITI {}

impl TOTO for TITI
{
    func print1() {}
}
