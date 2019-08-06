#module module484
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var res = T(0)
	@assert(res.0 == 1)
	@assert(res.1 == 2)
	@assert(res.2 == 3)
}

//#[swag.printbc]
func T(a: s32)-> {s32,s32,s32}
{
	return {1,2,3}
}
