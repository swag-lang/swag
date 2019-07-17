#module module443
//#unittest pass syntax

func F(a: s32, b: s32)
{
	@assert(a == 1)
	@assert(b == 0)
}

//#[swag.printbytecode]
#test
{
	F(b: 0, a: 1)
}