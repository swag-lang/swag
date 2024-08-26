#global testerror "Err0487"

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