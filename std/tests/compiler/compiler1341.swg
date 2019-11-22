interface TOTO
{
	print: func(string)
}

struct TITI
{
}

impl TOTO for TITI
{
    func print(using self, x: string)
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