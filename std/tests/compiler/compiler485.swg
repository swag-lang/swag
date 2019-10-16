


//#[swag.printbc]
#test
{
	var res = T()
	@assert(res.val0 == 1)
	@assert(res.val1 == 2)
	@assert(res.val2 == 3)
}

//#[swag.printbc]
func T()-> {s64,s64,s64}
{
	return {1's64,2's64,3's64}
}
