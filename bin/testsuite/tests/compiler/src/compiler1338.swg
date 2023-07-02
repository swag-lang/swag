interface TOTO
{
	func print(self)
    func print1(self, a: s32)
    func print2(self, a: bool)
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