#global testerror "Err0323"

interface TOTO
{
	func print(self, str: string);
}

struct TITI
{
}

struct TATA
{
}

impl TOTO for TITI
{
    func impl print(me: *TATA, x: string)
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