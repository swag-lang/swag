

//#[swag.printbc]
func lambda(x: f32)
{
	@assert(x == 666)
}

func lambda1(x: s32)
{
	@assert(x == 666)
}

//#[swag.printbc]
func(T) toto(x: func(T))
{
	x(666)
}

//#[swag.printbc]
#test
{
	toto(&lambda)
	toto(&lambda1)
}
