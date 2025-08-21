interface TOTO
{
	func print(me);
    func print1(me, x: s32);
}

struct TITI
{
}

impl TOTO for TITI
{
	func impl print(using me)
	{
	}

	func impl print1(using me, x: s32)
	{
	}
}