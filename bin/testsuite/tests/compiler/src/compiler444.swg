


#global testerror "Err0058"
func F(a: s32, b: s32)
{
	@assert(a == 1)
	@assert(b == 0)
}

//#[Swag.PrintBc]
#test
{
	F(a: 0, a: 1)
}