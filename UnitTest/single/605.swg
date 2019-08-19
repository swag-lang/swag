#module module605
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

//#[swag.printbc]
/*func toto()->Point
{
	var pt: Point
	pt.x = 100
	pt.y = 200
	return pt
}*/

#test
{
	var x : Point
	var y : Point
	x = y
}