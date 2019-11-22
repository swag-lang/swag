interface TOTO
{
	print: func()
}

struct TITI
{
}

impl TOTO for TITI
{
	func print(using self)
	{
	}
}

//#[swag.printbc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
}