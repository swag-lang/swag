


var g = 0

//#[swag.printbc]
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

//#[swag.printbc]
func F(param: s32, b: func(s32))
{
	b(param)	
}
