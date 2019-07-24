#module module484
//#unittest pass semantic

#[swag.printbytecode]
#test
{
	var res = T()
	@print(res.1)
}

#[swag.printbytecode]
func T()-> {s32, s32}
{
	return {1,2}
}
