#module "compiler1341"
#unittest error

interface TOTO
{
	print: func(string)
}

struct TITI
{
}

impl TOTO for TITI
{
    func print(y: bool, x: string)
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