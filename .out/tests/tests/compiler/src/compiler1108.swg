func(T) toto(x: T)
{
	@assert(@typeof(x) == f32)
}

#test
{
	toto'f32(1)
}