
/*
#[swag.printbc]
func lambda(x: f32)
{
	@print(x)
}

#[swag.printbc]
func(T) toto(x: (T))
{
	x(666)
}

//#[swag.printbc]
#test
{
	toto(&lambda)
}*/

func toto(x: f32)
{
	@assert(x == 666)
}

#test
{
	toto(666)
}