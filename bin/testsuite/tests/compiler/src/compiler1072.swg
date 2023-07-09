

#test
{
	var x = 5.0
	@assert(5 == x)

	var z = "null"
	@assert(null != z)

	var y = 5 * x
	@assert(@typeof(y) == f32)
	@assert(y == 25.0)
}