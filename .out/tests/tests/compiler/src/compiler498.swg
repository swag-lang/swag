


func T(a: const [..] s32)
{
	@assert(@countof(a) == 2)
	@assert(a[0] == 0)
	@assert(a[1] == 1)
}

//#[Swag.printbc]
#test
{
	T(@[0, 1])
}
