interface TOTO
{
	print: func(self)
    print1: func(self, s32)
    print2: func(self, bool)
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

    func impl print2(using self, x: bool)
	{
	}
}