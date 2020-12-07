struct point1
{
	x: s32
	y: s32
	z: s32
}

//#[swag.printbc]
struct point
{
	x: s32
	y: s32
	z: s32
	ptx: point1
}

//#[swag.printbc]
#test
{
	var pt : point
	//pt.ptx.opInit()
}