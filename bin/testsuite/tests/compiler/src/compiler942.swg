

//#[Swag.PrintBc]
#test
{
	var x: [2] any = @[6, "6"]

	t := x[0]
	type := @kindof(t)
	@assert(type.name == "s32")

	ptr := cast(*s32) @dataof(x[0])
	@assert(ptr[0] == 6)

	ptr1 := cast(*string) @dataof(x[1])
	@assert(ptr1[0] == "6")
}
