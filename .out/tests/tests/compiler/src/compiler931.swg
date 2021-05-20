

//#[Swag.printbc]
#test
{
	var x: any
	x = 666
	v := cast(*s32) @dataof(x)
	@assert(v[0] == 666)

	x = 666.0
	v1 := cast(*f32) @dataof(x)
	@assert(v1[0] == 666.0)
}
