//#[swag.printbc]
#test
{
	//var array = {1,2,3}
	//F(cast([..]s32) array, &T)
	c := @[666,667]
	F(cast([..]s32) c, &T)
	@assert(c[0] == 667)
	@assert(c[1] == 668)
}

//#[swag.printbc]
func T(param: *s32)
{
	param[0] += 1
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
