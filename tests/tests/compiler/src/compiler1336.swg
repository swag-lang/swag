interface TOTO
{
	print: func(self)
    print1: func(self, s32)
}

struct TITI
{
}

impl TOTO for TITI
{
	func print(using self)
	{
	}
    
	func print1(using self, x: s32)
	{
	}    
}