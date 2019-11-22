interface TOTO
{
	print: func()
    print1: func(s32)
    print2: func(bool)
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

    func print2(using self, x: bool)
	{
	}
}