


struct Point
{
	x: s32
	y: s32
}

struct Pair
{
	x: string
	y: f32 //Point
}

//#[swag.printbc]
#test
{
	//var pair: Pair("toto", {10,20})
	var pair: Pair{"toto", 1}
	@assert(pair.x == "toto")
	@assert(pair.y == 1.0)
	//@assert(pair.y.x == 10)
	//@assert(pair.y.y == 20)
	//var pair : {string, Point} = {"toto", {0, 0}}
	//var pair : {string, string} = {"toto", "toto"}
}
