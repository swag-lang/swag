

//#[Swag.PrintBc]
#test
{
	var x: any
	x = 666
	@assert(@kindof(x) == @typeof(s32))
	x = "666"
	@assert(@kindof(x) == @typeof(string))
}
