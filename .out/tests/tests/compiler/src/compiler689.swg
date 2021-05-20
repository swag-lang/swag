


struct(T) Point
{
	x: T = 5
	y: T = 10
}

//#[Swag.printbc]
#test
{
	var pt : Point'u64{100'u64, 500'u64}
	@assert(pt.x == 100)
	@assert(pt.y == 500)
}