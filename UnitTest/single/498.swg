#module module498
//#unittest pass semantic

func T(a: const [..] s32)
{
	@assert(@prop(a, count) == 2)
	@assert(a[0] == 0)
	@assert(a[1] == 1)
}

//#[swag.printbytecode]
#test
{
	T([0, 1])
}
