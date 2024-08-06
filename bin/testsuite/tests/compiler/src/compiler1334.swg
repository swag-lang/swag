#global testerror "Err0511"

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