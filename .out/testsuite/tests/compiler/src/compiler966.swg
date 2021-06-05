

struct Point
{
	x: f32
	y: f32
}

func toto(pt: Point)
{
	@assert(pt.x == 1)
	@assert(pt.y == 2)
}

//#[Swag.printbc]
#test
{
	toto(Point{1.0, 2.0})
}