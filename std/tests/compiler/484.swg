
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var res = T(0)
	@assert(res.val0 == 1)
	@assert(res.val1 == 2)
	@assert(res.val2 == 3)
}

//#[swag.printbc]
func T(a: s32)-> {s32,s32,s32}
{
	return {1,2,3}
}
