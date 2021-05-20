
using Swag

func toto(x: ...)
{
	@assert(cast(string) x[0] == "toto")
}

//#[Swag.printbc]
#test
{
	toto("toto")
}
