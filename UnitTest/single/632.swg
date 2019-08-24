#module module632
//#unittest pass semantic

struct(T) Point
{
	var x: T
	var y: T
}

#test
{
	var pt1: Point!(f32)
	var pt2: Point!(string)
	pt2.x = "0"
}