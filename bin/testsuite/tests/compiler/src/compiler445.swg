


#global testerror "Err0724"
func F(a: s32, b: s32)
{
	@assert(a == 1)
	@assert(b == 0)
}

//#[Swag.PrintBc]
#test
{
	F(a: 0, c: 1)
}