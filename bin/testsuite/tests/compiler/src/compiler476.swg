


var g = 0

//#[Swag.PrintBc]
#test
{
	F(&T)
	@assert(g == 1)
}

func T()
{
	g = 1
}

func F(b: func())
{
	b()
}
