#module module505
//#unittest pass semantic

//#[swag.printbytecode]
func T(xx: bool, a: ...)
{
	//@assert(@prop(a, count) == 3)
	@print(cast(s32) @prop(a, count))
}

//#[swag.printbytecode]
#test
{
	T(true, 10, 100, 666)
}
