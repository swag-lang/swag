struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pair : {string, Point} = @{"toto", {10, 20}}
	@assert(pair.item1.x == 10)
	@assert(pair.item1.y == 20)
	//var pair : {string, string} = {"toto", "toto"}
}
