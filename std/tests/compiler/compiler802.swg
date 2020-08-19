#module "compiler802"

struct Point
{
	x: s32
}

#unittest error
func toto(using pt: Point, using pt1: Point)
{
	x = 0
}

//#[swag.printbc]
#test
{
}