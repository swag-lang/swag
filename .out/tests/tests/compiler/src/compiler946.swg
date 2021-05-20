

//#[Swag.printbc]
func toto(x: string, y: ...)
{
	@assert(x == "1.0")

	ptr := cast(*string) @dataof(y[0])
	@assert(ptr[0] == "10")

	ptr1 := cast(*s32) @dataof(y[1])
	@assert(ptr1[0] == 20)
}

//#[Swag.printbc]
#test
{
	toto("1.0", "10", 20)
}
