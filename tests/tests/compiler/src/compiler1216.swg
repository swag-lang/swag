
#unittest error

struct Vector2
{
	x: f32
	y: f32
}

const XAxis: Vector2 = {1, 4, 5}

//#[swag.printbc]
#test
{
	@assert(XAxis.x == 1)
	//@assert(XAxis.y == 555)
}