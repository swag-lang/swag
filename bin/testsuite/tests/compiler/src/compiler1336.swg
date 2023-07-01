interface TOTO
{
	print: func(self)
    print1: func(self, s32)
}

struct TITI
{
}

impl TOTO for TITI
{
	func impl print(using self)
	{
	}

	func impl print1(using self, x: s32)
	{
	}
}