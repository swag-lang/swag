

#test
{
	x := 5.0
	@assert(5 == x)

	z := "null"
	@assert(null != z)

	y := 5 * x
	@assert(@typeof(y) == f32)
	@assert(y == 25.0)
}