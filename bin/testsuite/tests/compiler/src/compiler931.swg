

//#[Swag.PrintBc]
#test
{
	var x: any
	x = 666
	v := cast(*s32) @dataof(x)
	@assert(dref v == 666)

	x = 666.0
	v1 := cast(*f32) @dataof(x)
	@assert(dref v1 == 666.0)
}
