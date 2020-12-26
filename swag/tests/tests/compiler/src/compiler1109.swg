func(T = s32) toto(x: T)
{
	@assert(@typeof(x) == s32)
}

#test
{
	toto(1's32)
}