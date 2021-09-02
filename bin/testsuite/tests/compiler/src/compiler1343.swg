interface TOTO
{
	print: func(self, string)
}

struct TITI
{
}

impl TOTO for TITI
{
    func print(me: *TITI, x: string)
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