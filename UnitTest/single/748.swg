#module module748

func lambda(x: s32)
{
	@assert(x == 666)
}

func(T) toto(x: (T))
{
	x(666)
}

//#[swag.printbc]
#test
{
	toto(&lambda)
	//toto(666.0, &lambda)
}