#module "compiler1337"
#unittest error

interface TOTO
{
	print: func()
    print1: func(s32)
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