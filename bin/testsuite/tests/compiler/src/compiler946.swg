

//#[Swag.PrintBc]
func toto(x: string, y: ...)
{
	@assert(x == "1.0")

	ptr := cast(*string) @dataof(y[0])
	@assert(dref ptr == "10")

	ptr1 := cast(*s32) @dataof(y[1])
	@assert(dref ptr1 == 20)
}

//#[Swag.PrintBc]
#test
{
	toto("1.0", "10", 20)
}
