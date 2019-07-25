#module module488
//#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var res = T(0)
	@assert(res.0 == 1)
	@assert(res.1 == 2)
	@assert(res.2 == 3)
}

//#[swag.printbytecode]
func T(a: s32)-> {s32,s32,s32}
{
	return [1,2,3]
}
