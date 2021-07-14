

//#[Swag.PrintBc]
func average(x, y: any)
{
	tx:= cast(*s32) @dataof(x)
	@assert(tx[0] == 5)
	ty:= cast(*s32) @dataof(y)
	@assert(ty[0] == 10)
}

//#[Swag.PrintBc]
#test
{
	average(5, 10)
}
