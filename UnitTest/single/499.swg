#module module499
//#unittest pass semantic

#unittest error
func T(a: ..., c: s32)
{
}

//#[swag.printbytecode]
#test
{
	T(0, 1)
}
