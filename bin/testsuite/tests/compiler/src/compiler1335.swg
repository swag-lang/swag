#global testerror "Err0362"

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