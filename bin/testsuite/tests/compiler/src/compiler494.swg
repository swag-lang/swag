


func T()->string { return "toto"; }

//#[Swag.PrintBc]
#test
{
	@assert(T() == "toto")
}
