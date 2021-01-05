
#global testerror

interface TOTO
{
	print: func(s32)
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