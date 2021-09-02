func lambda(x: s32)
{
	@assert(x == 666)
}

func(T) toto(val: T, x: func(T))
{
	x(val)
}

//#[Swag.PrintBc]
#test
{
	toto(666's32, &lambda)
	//toto(666.0, &lambda)
}