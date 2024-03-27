


func F(a: string = "toto")
{
	@assert(a == "toto")
}

//#[Swag.PrintBc]
#test
{
	F()
}