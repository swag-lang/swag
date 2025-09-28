struct point1
{
	x: s32
	y: s32
	z: s32
}

//#[Swag.PrintBc]
struct point
{
	x: s32
	y: s32
	z: s32
	ptx: point1
}

//#[Swag.PrintBc]
#test
{
	var pt : point
	//pt.ptx.opInit()
}