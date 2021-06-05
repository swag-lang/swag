


#global testerror
func F(a: s32, b: s32)
{
	@assert(a == 1)
	@assert(b == 0)
}

//#[Swag.printbc]
#test
{
	F(a: 0, a: 1)
}