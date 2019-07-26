#module module500
//#unittest pass semantic

func T(a: ...)
{
}

//#[swag.printbytecode]
#test
{
	T(0, 1)
}
