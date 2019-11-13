


var g = 0

//#[swag.printbc]
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
