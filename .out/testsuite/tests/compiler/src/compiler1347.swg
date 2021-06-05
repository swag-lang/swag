interface TOTO
{
	print: func(*TOTO, bool)
}

struct TITI
{
}

impl TOTO for TITI
{
    //#[Swag.printbc]
    func print(using self, x: bool)
    {
        //@print("x")
    }
}

//#[Swag.printbc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
    //x.print(true)
}