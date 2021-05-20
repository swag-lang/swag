


#global testerror
func F(a: s32 = 10, b: s32 = 20)
{
	@assert(a == 10)
	@assert(b == 666)
}

//#[Swag.printbc]
#test
{
	F(b: "toto")
}