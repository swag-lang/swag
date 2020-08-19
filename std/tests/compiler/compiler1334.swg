#module "compiler1334"
#unittest error

interface TOTO
{
	print: func()
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