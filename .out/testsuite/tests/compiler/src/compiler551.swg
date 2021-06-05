


struct point1
{
	x = 10
	y = 20
}

struct point
{
	pt: point1
	x = "10"
	y = "20"
}

//#[Swag.printbc]
#test
{
	var pt: [10] point
	ptr1 := &pt[9]
	ptr1.pt.x = 100
	@assert(ptr1.pt.x == 100)
}