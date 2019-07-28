#module module508
#unittest pass semantic

//#[swag.printbytecode]
func T(xx: string, a: ...)
{
	x0 := cast(s32) a[0]
	@assert(x0 == 10)
	x1 := cast(string) a[1]
	@assert(x1 == "toto")
	x2 := cast(s32) a[2]
	@assert(x2 == 600)
}

//#[swag.printbytecode]
#test
{
	T("true", 10, "toto", 600)
}
