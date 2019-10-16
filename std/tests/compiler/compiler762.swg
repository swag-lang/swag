#module compiler762

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#unittest error
#test
{	
	cpt := Point
}
