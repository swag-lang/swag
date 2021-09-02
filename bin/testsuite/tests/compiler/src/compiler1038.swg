

struct Point
{
}

//#[Swag.PrintBc]
#test
{
	var pt : [5] Point
	ptr := &pt
	@init(ptr, 5)
}