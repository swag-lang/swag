
using swag

func toto(x: ...)
{	
	@assert(cast(string) x[0] == "toto")
}

//#[swag.printbc]
#test
{
	toto("toto")
}
