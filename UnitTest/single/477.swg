#module module477
#unittest pass semantic

var g = 0

//#[swag.printbytecode]
#test
{
	F(666, &T)
	@assert(g == 666)
	F(667, &T)
	@assert(g == 667)
}

func T(param: s32)
{
	g = param
}

//#[swag.printbytecode]
func F(param: s32, b: (s32))
{
	b(param)	
}
