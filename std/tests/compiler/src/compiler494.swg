


func T()->string { return "toto"; }

//#[swag.printbc]
#test
{
	@assert(T() == "toto")
}
