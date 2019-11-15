struct point1
{
	x: s32 = 100
	y: s32 = 200
	z: s32 = 300
}

struct point
{
	in: point1
	x: s32 = 10
	y: s32 = 20
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(@sizeof(pt) == 20)
}