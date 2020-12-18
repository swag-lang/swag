


func F(a: string = "toto")
{
	@assert(a == "toto")
}

//#[swag.printbc]
#test
{
	F()
}