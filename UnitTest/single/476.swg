#module module476
#unittest pass semantic

var g = 0

//#[swag.printbytecode]
#test
{
	F(&T)
	@assert(g == 1)
}

func T()
{
	g = 1
}

func F(b: ())
{
	b()	
}
