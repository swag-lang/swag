struct Point
{
	x: f32
	y: f32
}

//#[Swag.printbc]
#test
{
	var toto: Point = @{1.0, 2.0}
	@assert(toto.x == 1)
	@assert(toto.y == 2)
}