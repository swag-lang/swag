

//#[Swag.PrintBc]
func average(x, y: any)
{
	var tx = cast(*s32) @dataof(x)
	@assert(dref tx == 5)
	var ty = cast(*s32) @dataof(y)
	@assert(dref ty == 10)
}

//#[Swag.PrintBc]
#test
{
	average(5, 10)
}
