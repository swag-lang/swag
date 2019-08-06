#module module542
//#unittest pass semantic

var a: bool = false;
var b: bool = false;

namespace toto
{
	func F()
	{
		a = true
	}
}

namespace titi
{
	func F()
	{
		b = true
	}
}

//#[swag.printbc]
#test
{	
	@assert(a == false)
	@assert(b == false)
	toto.F
	titi.F
	@assert(a == true)
	@assert(b == true)
}