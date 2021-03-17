
#global testerror

struct Vector2
{
	x: f32 = 666
	y: f32
}

const XAxis: Vector2(y: 1.0, 1)

#test
{
	@assert(XAxis.x == 666.0)
    @assert(XAxis.y == 1.0)
}
