

//#[Swag.PrintBc]
func lambda(x: f32)
{
	@assert(x == 666)
}

func lambda1(x: s32)
{
	@assert(x == 666)
}

//#[Swag.PrintBc]
func(T) toto(x: func(T))
{
	x(666)
}

//#[Swag.PrintBc]
#test
{
	toto(&lambda)
	toto(&lambda1)
}
