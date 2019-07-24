#module module484
//#unittest pass semantic

#[swag.printbytecode]
#test
{
	var res = T()
	@assert(res.0 == 1)
	@assert(res.1 == 2)
	@assert(res.2 == 3)
}

#[swag.printbytecode]
func T()-> {s32,s32,s32}
{
	return {1,2,3}
}
