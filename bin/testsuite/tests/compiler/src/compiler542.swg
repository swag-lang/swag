


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

//#[Swag.PrintBc]
#test
{
	a = false
	b = false
	@assert(a == false)
	@assert(b == false)
	toto.F()
	titi.F()
	@assert(a == true)
	@assert(b == true)
}