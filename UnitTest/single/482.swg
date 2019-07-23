#module module482
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var res = T()
}

//#[swag.printbytecode]
func T()-> {s32, s32}
{
	return {1,2}
}
