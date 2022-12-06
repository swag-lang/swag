

//#[Swag.PrintBc]
#test
{
	var x: [2] any = [6, "6"]

	t := x[0]
	type := @kindof(t)
	@assert(type.fullname == "s32")

	ptr := cast(*s32) @dataof(x[0])
	@assert(dref ptr == 6)

	ptr1 := cast(*string) @dataof(x[1])
	@assert(dref ptr1 == "6")
}
