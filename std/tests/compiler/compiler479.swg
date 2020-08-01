


var g = 0

//#[swag.printbc]
#test
{
	g = 0

	var array = @[1,2,3]
	F(cast([..]s32) array, &T)
	@assert(g == 6)
}

func T(param: *s32)
{
	g += param[0]
}

//#[swag.printbc]
func F(param: [..] s32, b: func(*s32))
{
	var data = @dataof(param)
	loop(param)
	{
		b(data + @index)
	}
}
