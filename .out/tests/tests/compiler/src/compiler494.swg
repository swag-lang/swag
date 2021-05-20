


func T()->string { return "toto"; }

//#[Swag.printbc]
#test
{
	@assert(T() == "toto")
}
