

//#[swag.printbc]
#test
{
	var x: [] any = @[6]

	t := x[0]
	type := @kindof(t)
	@assert(type.name == "s32")

	ptr := cast(*s32) @dataof(x[0])
	@assert(ptr[0] == 6)
}
