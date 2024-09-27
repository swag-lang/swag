

func lambda(x: s32)
{
	@assert(x == 666)
}

func(T) toto(x: nn func(T))
{
	x(666)
}

//#[Swag.PrintBc]
#test
{
	toto(&lambda)
	//toto(666.0, &lambda)
}