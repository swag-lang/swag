#module module483
//#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var res = T()
	b := 1
	@print(res.2)
}

//#[swag.printbytecode]
func T()-> {s32, s32}
{
	return {1,2}
}
