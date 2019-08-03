#module module551
//#unittest pass semantic

struct point1
{
	var x = 10
	var y = 20
}

struct point
{
	var pt: point1
	var x = "10"
	var y = "20"
}

//#[swag.printbytecode]
#test
{
	var pt: [10] point
	ptr1 := &pt[9]
	ptr1.pt.x = 100	
	@assert(ptr1.pt.x == 100)
}