struct Vector2
{
	x, y: f32
}

const XAxis: Vector2{444, 555}
const YAxis: Vector2{10, 666}

//#[swag.printbc]
#test
{
	var pt1: Vector2 = XAxis
	@assert(pt1.x == 444)
	@assert(pt1.y == 555)

	var pt2: Vector2 = YAxis
	@assert(pt2.x == 10)
	@assert(pt2.y == 666)
}