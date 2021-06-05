struct Point
{
	x: s32
	y: s32
	z: s32
}

//#[Swag.printbc]
func toto()->Point
{
	var pt: Point
	pt.x = 100
	pt.y = 200
	pt.z = 300
	return pt
}

//#[Swag.printbc]
#test
{
	var a : Point
	a = toto()
	@assert(a.x == 100)
	@assert(a.y == 200)
	@assert(a.z == 300)
}