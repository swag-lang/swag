


struct point
{
	x: s32 = 10
	y: s32 = 100
}

impl point
{
}

//#[Swag.PrintBc]
#test
{
	var pt: point
	//@assert(pt.x == 10)
}
