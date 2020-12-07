#module "compiler447"


#unittest error
func F(a: s32 = 10, b: s32 = 20)
{
	@assert(a == 10)
	@assert(b == 666)
}

//#[swag.printbc]
#test
{
	F(b: "toto")
}