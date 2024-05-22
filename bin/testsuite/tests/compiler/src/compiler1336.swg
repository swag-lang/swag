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

	func impl print1(using self, x: s32)
	{
	}
}