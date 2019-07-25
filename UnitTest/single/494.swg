#module module494
//#unittest pass semantic

func T()->string { return "toto"; }

//#[swag.printbytecode]
#test
{
	@assert(T == "toto")
}
