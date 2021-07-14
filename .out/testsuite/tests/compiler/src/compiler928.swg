

var G : s32 = 0

//#[Swag.PrintBc]
func average(x: any)
{
	tt := @kindof(x)
	if(tt == @typeof(s32))
	{
		val := cast(*s32) @dataof(x)
		@assert(val[0] == 666)
		G += 1
	}
	else if(tt == @typeof(string))
	{
		val := cast(*string) @dataof(x)
		@assert(val[0] == "666")
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
