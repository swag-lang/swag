#module "compiler445"


#unittest error
func F(a: s32, b: s32)
{
	@assert(a == 1)
	@assert(b == 0)
}

//#[swag.printbc]
#test
{
	F(a: 0, c: 1)
}