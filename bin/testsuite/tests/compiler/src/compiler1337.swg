#global testerror "Err0294"

interface TOTO
{
	func print(self)
    print1: func(self,s32)
}

struct TITI
{
}

impl TOTO for TITI
{
	func print(using self)
	{
	}
}