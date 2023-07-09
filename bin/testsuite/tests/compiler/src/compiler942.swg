

//#[Swag.PrintBc]
#test
{
	var x: [2] any = [6, "6"]

	var t = x[0]
	var type = @kindof(t)
	@assert(type.fullname == "s32")

	var ptr = cast(*s32) @dataof(x[0])
	@assert(dref ptr == 6)

	var ptr1 = cast(*string) @dataof(x[1])
	@assert(dref ptr1 == "6")
}
