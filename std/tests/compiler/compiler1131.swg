func(T) toto(x, y: T)
{
	@assert(x == y)
	@assert(x is f32)
	@assert(y is f32)
}

//#[swag.printbc]
#test
{
	toto(1.0'f32, 1)
}