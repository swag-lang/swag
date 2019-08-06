#module module454
//#unittest pass syntax

func F(a: string = "toto")
{
	@assert(a == "toto")
}

//#[swag.printbc]
#test
{
	F()
}