#module module931

func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[swag.printbc]
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
