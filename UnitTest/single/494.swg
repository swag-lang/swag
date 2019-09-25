#module module494
//#unittest pass semantic

func T()->string { return "toto"; }

//#[swag.printbc]
#test
{
	@assert(T() == "toto")
}
