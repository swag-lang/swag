#global testerror

interface TOTO
{
	print: func(self, string)
}

struct TITI
{
}

impl TOTO for TITI
{
    func print(y: bool, x: string)
    {
    }
}
