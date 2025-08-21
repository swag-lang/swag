interface TOTO
{
	func print(me);
    func print1(me, a: s32);
    func print2(me, a: bool);
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

    func impl print2(using me, x: bool)
	{
	}
}