func(T) toto(x, y: T)
{
	@assert(x == y)
	@assert(@typeof(x) == f32)
	@assert(@typeof(x) == f32)
}

//#[swag.printbc]
#test
{
	toto(1.0'f32, 1)
}