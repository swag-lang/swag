#global testerror "Err0652"

interface TOTO
{
	func print(self, x: s32)
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