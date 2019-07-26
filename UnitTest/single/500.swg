#module module500
//#unittest pass semantic

//#[swag.printbytecode]
func T(a: ...)
{
}

//#[swag.printbytecode]
#test
{
	T(10, 100)
	T(10, "100")
}
