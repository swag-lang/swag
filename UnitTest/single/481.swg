#module module481
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	@print("go go go\n")
	//var array = {1,2,3}
	//F(cast([..]s32) array, &T)
	c := {666,667}
	F(cast([..]s32) c, &T)
	//@assert(c[0] == 667)
	//@assert(c[1] == 668)
}

#[swag.printbytecode]
func T(param: *s32)
{
	@print(param[0])
	@print(" ")
	param[0] += 1
	@print(param[0])
	@print("\n")
}

//#[swag.printbytecode]
func F(param: [..] s32, b: (*s32))
{
	@print(cast(s32) @prop(param, count))
	@print("\n")
	var data = @prop(param, data)
	loop(param)
	{
		b(data + index)	
	}
}
