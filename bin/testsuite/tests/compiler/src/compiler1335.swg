#global testerror "Syn0133"

interface TOTO
{
	print: func(self, s32)
}

struct TITI
{
}

impl TOTO for TITI
{
	func print()
	{
	}
}