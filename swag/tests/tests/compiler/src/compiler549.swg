


struct point
{
	x = "10"
	y = "20"
}

//#[swag.printbc]
#test
{
	var pt: [10] point
	ptr := &pt[0]
	@assert(ptr.x == "10")
	@assert(ptr.y == "20")
	ptr1 := &pt[9]
	@assert(ptr1.x == "10")
	@assert(ptr1.y == "20")
}