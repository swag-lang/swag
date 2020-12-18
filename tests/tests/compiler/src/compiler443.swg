


func F(a: s32, b: s32)
{
	@assert(a == 124)
	@assert(b == 666)
}

//#[swag.printbc]
#test
{
	F(b: 666, a: 124)
}