#global testerror "Err0508"

interface TOTO
{
	func print(self);
}

struct TITI
{
}

impl TOTO for TITI
{
	func print1()
	{
	}
}