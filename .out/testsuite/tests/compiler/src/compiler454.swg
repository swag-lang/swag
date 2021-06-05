


func F(a: string = "toto")
{
	@assert(a == "toto")
}

//#[Swag.printbc]
#test
{
	F()
}