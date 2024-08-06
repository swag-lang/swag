#global testerror "Err0370"

interface TOTO
{
	func print(self, str: string);
}

struct TITI
{
}

impl TOTO for TITI
{
    func impl print(y: bool, x: string)
    {
    }
}
