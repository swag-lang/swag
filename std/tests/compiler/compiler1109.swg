func(T = s32) toto(x: T)
{
	@assert(x is s32)
}

#test
{
	toto(1's32)
}