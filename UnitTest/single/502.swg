#module module502
#unittest pass semantic

//#[swag.printbytecode]
#unittest error
func T(xx: string, a: ...)
{
	@assert(@prop(a, count) == 3)
	@print(cast(s32) @prop(a, count))
}

//#[swag.printbytecode]
#test
{
	T(true, 10, 100, 1000)
}
