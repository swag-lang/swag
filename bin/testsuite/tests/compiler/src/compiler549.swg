


struct point
{
	x = "10"
	y = "20"
}

//#[Swag.PrintBc]
#test
{
	var pt: [10] point
	let ptr = &pt[0]
	@assert(ptr.x == "10")
	@assert(ptr.y == "20")
	let ptr1 = &pt[9]
	@assert(ptr1.x == "10")
	@assert(ptr1.y == "20")
}