#global testerror "Err0463"

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