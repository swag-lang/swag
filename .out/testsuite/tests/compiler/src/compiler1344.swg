interface TOTO
{
	print: func(self, string)
}

struct TITI
{
}

impl TOTO for TITI
{
    func print(using self, x: string)
    {
        //@print(x)
    }
}

//#[Swag.printbc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
    //y.print("toto\n")
}