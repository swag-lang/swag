
#global testerror

interface TOTO
{
	print: func(self, string)
}

struct TITI
{
}

struct TATA
{
}

impl TOTO for TITI
{
    func print(me: *TATA, x: string)
    {
    }
}

//#[Swag.PrintBc]
#test
{
/*    var x: TOTO
    var y: TITI
    x = y*/
}