

func lambda(x: s32)
{
	@assert(x == 666)
}

func(T) toto(val: T, x: (T))
{
	x(val)
}

//#[swag.printbc]
#test
{
	toto(666, &lambda)
	//toto(666.0, &lambda)
}