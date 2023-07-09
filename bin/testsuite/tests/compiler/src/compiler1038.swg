

struct Point
{
}

//#[Swag.PrintBc]
#test
{
	var pt : [5] Point
	var ptr = &pt
	@init(ptr, 5)
}