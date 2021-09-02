#global testerror

interface TOTO
{
	print: func(self)
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