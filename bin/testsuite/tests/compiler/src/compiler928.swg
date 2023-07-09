

var G : s32 = 0

//#[Swag.PrintBc]
func average(x: any)
{
	var tt = @kindof(x)
	if(tt == @typeof(s32))
	{
		var val = cast(*s32) @dataof(x)
		@assert(dref val == 666)
		G += 1
	}
	elif(tt == @typeof(string))
	{
		var val = cast(*string) @dataof(x)
		@assert(dref val == "666")
		G += 1
	}
}

//#[Swag.PrintBc]
#test
{
	G = 0
	average(666)
	average("666")
	@assert(G == 2)
}
