#module module478
//#unittest pass semantic
/*
#[swag.printbc]
#test
{
	var array = {1,2,3}
	F(array, &T)
}

func T(param: *s32)
{
	@print(param[0])
}

#[swag.printbc]
func F(param: [3] s32, b: (*s32))
{
	var data = @prop(param, data)
	loop(param)
	{
		b(data + index)	
	}
}*/

//#[swag.printbc]
#test
{
	var array = [1,2,3]
	F(cast([..]s32) array)
}

func F(param: [..] s32)
{
	@assert(@prop(param, count) == 3)
}