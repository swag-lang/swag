

var x: [] any = @[6]

//#[Swag.PrintBc]
#test
{
	t := x[0]
	type := @kindof(t)
	@assert(type.fullname == "s32")

	ptr := cast(*s32) @dataof(x[0])
	@assert(ptr[0] == 6)
}
