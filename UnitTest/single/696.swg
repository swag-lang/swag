#module module696
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 10
}

struct(T) Array
{
	var pt: Point!T
}

//#[swag.printbc]
#test
{
	var arr: Array!f32
	@assert(arr.pt.x == 5.0)
	@assert(arr.pt.y == 10.0)
}