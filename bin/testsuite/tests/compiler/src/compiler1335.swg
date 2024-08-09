#global testerror "Err0371"

interface TOTO
{
	func print(self, x: s32);
}

struct TITI
{
}

impl TOTO for TITI
{
	func impl print()
	{
	}
}