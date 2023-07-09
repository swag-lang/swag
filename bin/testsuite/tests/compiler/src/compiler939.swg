

var x: [] any = [6]

//#[Swag.PrintBc]
#test
{
	var t = x[0]
	var type = @kindof(t)
	@assert(type.fullname == "s32")

	var ptr = cast(*s32) @dataof(x[0])
	@assert(dref ptr == 6)
}
