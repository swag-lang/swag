#module module750

#[swag.printbc]
func lambda(x: f32)
{
	@assert(x == 666)
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
}
