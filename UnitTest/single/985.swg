#module module985
//#unittest pass semantic

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var pair : {string, Point} = {"toto", {10, 20}}
	@assert(pair.val1.x == 10)
	@assert(pair.val1.y == 20)
	//var pair : {string, string} = {"toto", "toto"}
}

