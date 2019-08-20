#module module608
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
	var z: s32
}

//#[swag.printbc]
func toto()->Point
{
	var pt: Point
	pt.x = 100
	pt.y = 200
	pt.z = 300
	return pt
}

//#[swag.printbc]
#test
{
	var a : Point
	a = toto()
	@assert(a.x == 100)
	@assert(a.y == 200)
	@assert(a.z == 300)
}