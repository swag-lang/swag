func(T) toto(x: T)
{
	@assert(x is f32)
}

#test
{
	toto'f32(1)
}