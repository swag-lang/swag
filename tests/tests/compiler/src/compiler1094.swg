#test
{
	x := 1
	//@assert(x & 1)
	@assert(x & 1 != 0)
	@assert(x | 1 != 0)
	@assert(x & 2 == 0)
}
