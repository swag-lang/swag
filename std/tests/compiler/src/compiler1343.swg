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

//#[swag.printbc]
#test
{
/*    var x: TOTO
    var y: TITI
    x = y*/
}