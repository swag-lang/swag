//#[swag.printbc]
func toto(x: f32, y: ...)
{	
	@assert(x == 1.0)
	
	result := 0
	loop @countof(y)
	{
		p := y[@index]
		ptr := cast(*s32) @dataof(p)
		result += ptr[0]
	}
	
	@assert(result == 60)
}

//#[swag.printbc]
#test
{
	toto(1.0, 10, 20, 30)
}
