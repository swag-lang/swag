struct Vector2
{
	x: f32 = 666
	y: f32
}

#test
{
	var XAxis: Vector2{y: 1.0}

	@assert(XAxis.x == 666.0)
    @assert(XAxis.y == 1.0)
}
