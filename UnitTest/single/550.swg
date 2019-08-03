#module module550
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
	ptr := &pt[0]
	@assert(ptr.x == "10")
	@assert(ptr.y == "20")
	ptr1 := &pt[9]
	@assert(ptr1.x == "10")
	@assert(ptr1.y == "20")
	
	@assert(ptr1.pt.x == 10)
	@assert(ptr1.pt.y == 20)
}