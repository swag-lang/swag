#global testerror "Err0447"

interface TOTO
{
	func print(self);
    func print1(self, x: s32);
}

struct TITI
{
}

impl TOTO for TITI
{
	func impl print(using self)
	{
	}
}