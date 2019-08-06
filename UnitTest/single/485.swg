#module module485
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var res = T()
	@assert(res.0 == 1)
	@assert(res.1 == 2)
	@assert(res.2 == 3)
}

//#[swag.printbc]
func T()-> {s64,s64,s64}
{
	return {1's64,2's64,3's64}
}
