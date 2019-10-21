func(T) toto(x: T)
{
	@assert(x is s64)
}

#test
{
	toto!s64(1)
}