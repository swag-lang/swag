

struct Point
{
}

//#[Swag.printbc]
#test
{
	var pt : [5] Point
	ptr := &pt
	@init(ptr, 5)
}