#module module534
#unittest pass semantic

func toto(x: s32)
{
	@assert(x == 666)
}

//#[swag.printbytecode]
#test
{
	a := 666
	a.toto()
}