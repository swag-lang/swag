interface TOTO
{
	print: func(*TOTO, bool)
}

struct TITI
{
}

impl TOTO for TITI
{
    //#[Swag.PrintBc]
    func print(using self, x: bool)
    {
        //@print("x")
    }
}

//#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
    //x.print(true)
}