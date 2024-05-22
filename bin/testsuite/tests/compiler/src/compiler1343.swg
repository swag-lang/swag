interface TOTO
{
	func print(self, x: string);
}

struct TITI
{
}

impl TOTO for TITI
{
    func impl print(me: *TITI, x: string)
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