#module module501
#unittest pass semantic

//#[swag.printbc]
func T(a: ...)
{
	var xx: s32;
	@assert(@prop(a, count) == 3)
}

//#[swag.printbc]
#test
{
	T(10, 100, 1000)
}
