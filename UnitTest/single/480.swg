#module module480
#unittest pass semantic

//#[swag.printbc]
#test
{
	//var array = {1,2,3}
	//F(cast([..]s32) array, &T)
	c := [666,667]
	T(&c[1])
	@assert(c[0] == 666)
	@assert(c[1] == 668)
}

//#[swag.printbc]
func T(param: *s32)
{
	param[0] += 1
}

//#[swag.printbc]
/*func F(param: [..] s32, b: (*s32))
{
	var data = @dataof(param)
	loop(param)
	{
		b(data + index)	
	}
}*/
