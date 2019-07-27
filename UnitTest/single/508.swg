#module module508
//#unittest pass semantic

//#[swag.printbytecode]
func T(xx: bool, a: ...)
{
	//x := cast(s32) a[0]
}

//#[swag.printbytecode]
#test
{
	T(true, 10, "100", 600)
}
